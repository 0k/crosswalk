// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/extensions/browser/xwalk_extension_service.h"

#include "base/callback.h"
#include "base/command_line.h"
#include "base/scoped_native_library.h"
#include "base/synchronization/lock.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "ipc/ipc_message_macros.h"
#include "xwalk/extensions/common/xwalk_extension.h"
#include "xwalk/extensions/common/xwalk_extension_messages.h"
#include "xwalk/extensions/common/xwalk_extension_server.h"
#include "xwalk/extensions/common/xwalk_extension_switches.h"

using content::BrowserThread;

namespace xwalk {
namespace extensions {

namespace {

XWalkExtensionService::RegisterExtensionsCallback
g_register_extensions_callback;

base::FilePath g_external_extensions_path_for_testing_;

}

// This object intercepts messages destined to a XWalkExtensionServer and
// dispatch them to its task runner. A message loop proxy of a thread is a
// task runner. Like other filters, this filter will run in the IO-thread.
//
// In the case of in process extensions, we will pass the task runner of the
// extension thread.
class ExtensionServerMessageFilter : public IPC::ChannelProxy::MessageFilter {
 public:
  ExtensionServerMessageFilter(
      scoped_refptr<base::SequencedTaskRunner> task_runner,
      XWalkExtensionServer* extension_thread_server,
      XWalkExtensionServer* ui_thread_server)
      : task_runner_(task_runner),
        extension_thread_server_(extension_thread_server),
        ui_thread_server_(ui_thread_server) {}

  // Tells the filter to stop dispatching messages to the server.
  void Invalidate() {
    base::AutoLock l(lock_);
    task_runner_ = NULL;
    extension_thread_server_ = NULL;
    ui_thread_server_ = NULL;
  }

 private:
  virtual ~ExtensionServerMessageFilter() {}

  // IPC::ChannelProxy::MessageFilter implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE {
    if (IPC_MESSAGE_CLASS(message) == XWalkExtensionClientServerMsgStart) {
      base::AutoLock l(lock_);
      if (!extension_thread_server_)
        return false;
      task_runner_->PostTask(
          FROM_HERE,
          base::Bind(
              base::IgnoreResult(&XWalkExtensionServer::OnMessageReceived),
              base::Unretained(extension_thread_server_), message));
      return true;
    }
    return false;
  }

  // This lock is used to protect access to filter members.
  base::Lock lock_;

  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  XWalkExtensionServer* extension_thread_server_;
  XWalkExtensionServer* ui_thread_server_;
};

XWalkExtensionService::XWalkExtensionService(XWalkExtensionService::Delegate*
    delegate)
    : extension_thread_("XWalkExtensionThread"),
      delegate_(delegate) {
  if (!g_external_extensions_path_for_testing_.empty())
    external_extensions_path_ = g_external_extensions_path_for_testing_;
  registrar_.Add(this, content::NOTIFICATION_RENDERER_PROCESS_TERMINATED,
                 content::NotificationService::AllBrowserContextsAndSources());

  // IO main loop is needed by extensions watching file descriptors events.
  base::Thread::Options options(base::MessageLoop::TYPE_IO, 0);
  extension_thread_.StartWithOptions(options);
}

XWalkExtensionService::~XWalkExtensionService() {
  // This object should have been released and asked to be deleted in the
  // extension thread.
  if (!extension_data_map_.empty())
    VLOG(1) << "The ExtensionData map is not empty!";
}

void XWalkExtensionService::RegisterExternalExtensionsForPath(
    const base::FilePath& path) {
  external_extensions_path_ = path;
}

void XWalkExtensionService::OnRenderProcessHostCreated(
    content::RenderProcessHost* host) {
  CHECK(host);

  ExtensionData* data = new ExtensionData;
  CreateInProcessExtensionServers(host, data);

  CommandLine* cmd_line = CommandLine::ForCurrentProcess();
  if (!cmd_line->HasSwitch(switches::kXWalkDisableExtensionProcess))
    CreateExtensionProcessHost(host, data);
  else if (!external_extensions_path_.empty()) {
    RegisterExternalExtensionsInDirectory(
        data->in_process_extension_thread_server_.get(),
        external_extensions_path_);
  }

  extension_data_map_[host->GetID()] = data;
}

// static
void XWalkExtensionService::SetRegisterExtensionsCallbackForTesting(
    const RegisterExtensionsCallback& callback) {
  g_register_extensions_callback = callback;
}

// static
void XWalkExtensionService::SetExternalExtensionsPathForTesting(
    const base::FilePath& path) {
  g_external_extensions_path_for_testing_ = path;
}

XWalkExtensionService::ExtensionData::ExtensionData() {
}

XWalkExtensionService::ExtensionData::~ExtensionData() {
}

// We use this to keep track of the RenderProcess shutdown events.
// This is _very_ important so we can clean up all we need gracefully,
// avoiding invalid IPC steps after the IPC channel is gonne.
void XWalkExtensionService::Observe(int type,
                              const content::NotificationSource& source,
                              const content::NotificationDetails& details) {
  switch (type) {
    case content::NOTIFICATION_RENDERER_PROCESS_TERMINATED:
    case content::NOTIFICATION_RENDERER_PROCESS_CLOSED: {
      content::RenderProcessHost* rph =
          content::Source<content::RenderProcessHost>(source).ptr();
      OnRenderProcessHostClosed(rph);
    }
  }
}

void XWalkExtensionService::OnRenderProcessHostClosed(
    content::RenderProcessHost* host) {
  ExtensionData* data = extension_data_map_[host->GetID()];
  // Invalidate the objects in the different threads so they stop posting
  // messages to each other. This is important because we'll schedule the
  // deletion of both objects to their respective threads.
  ExtensionServerMessageFilter* message_filter =
      data->in_process_message_filter_;
  CHECK(message_filter);

  message_filter->Invalidate();

  scoped_ptr<XWalkExtensionServer> extension_thread_server =
      data->in_process_extension_thread_server_.Pass();
  scoped_ptr<XWalkExtensionServer> ui_thread_server =
      data->in_process_ui_thread_server_.Pass();

  CHECK(extension_thread_server);
  CHECK(ui_thread_server);

  extension_thread_server->Invalidate();
  ui_thread_server->Invalidate();

  // This will cause the filter to be deleted in the IO-thread.
  host->GetChannel()->RemoveFilter(message_filter);

  extension_thread_.message_loop()->DeleteSoon(
      FROM_HERE, extension_thread_server.release());

  scoped_ptr<XWalkExtensionProcessHost> eph =
      data->extension_process_host_.Pass();

  if (eph)
    BrowserThread::DeleteSoon(BrowserThread::IO, FROM_HERE, eph.release());

  extension_data_map_.erase(host->GetID());
}

void XWalkExtensionService::CreateInProcessExtensionServers(
    content::RenderProcessHost* host, ExtensionData* data) {
  scoped_ptr<XWalkExtensionServer> extension_thread_server(
      new XWalkExtensionServer);
  scoped_ptr<XWalkExtensionServer> ui_thread_server(
      new XWalkExtensionServer);

  IPC::ChannelProxy* channel = host->GetChannel();

  ExtensionServerMessageFilter* message_filter =
      new ExtensionServerMessageFilter(extension_thread_.message_loop_proxy(),
                                       extension_thread_server.get(),
                                       ui_thread_server.get());
  channel->AddFilter(message_filter);

  extension_thread_server->Initialize(channel);
  ui_thread_server->Initialize(channel);

  // The filter is owned by the IPC channel but we keep a reference to remove
  // it from the Channel later during a RenderProcess shutdown.
  data->in_process_message_filter_ = message_filter;

  delegate_->RegisterInternalExtensionsInExtensionThreadServer(
      extension_thread_server.get());
  delegate_->RegisterInternalExtensionsInUIThreadServer(
      ui_thread_server.get());

  if (!g_register_extensions_callback.is_null())
    g_register_extensions_callback.Run(extension_thread_server.get());

  data->in_process_extension_thread_server_ = extension_thread_server.Pass();
  data->in_process_ui_thread_server_ = ui_thread_server.Pass();
}

void XWalkExtensionService::CreateExtensionProcessHost(
    content::RenderProcessHost* host, ExtensionData* data) {
  data->extension_process_host_ = make_scoped_ptr(
      new XWalkExtensionProcessHost(host, external_extensions_path_, this));
}

void XWalkExtensionService::OnExtensionProcessDied(
    XWalkExtensionProcessHost* eph, int render_process_id) {
  // When this is called it means that XWalkExtensionProcessHost is about
  // to be deleted. We should invalidate our reference to it so we avoid a
  // segfault when trying to delete it within
  // XWalkExtensionService::OnRenderProcessHostClosed();

  ExtensionData* data = extension_data_map_[render_process_id];

  if (data) {
    XWalkExtensionProcessHost* extension_process_host =
        data->extension_process_host_.release();

    CHECK(extension_process_host == eph);
  }
}

}  // namespace extensions
}  // namespace xwalk
