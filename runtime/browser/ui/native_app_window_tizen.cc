// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/runtime/browser/ui/native_app_window_tizen.h"

#include "base/memory/scoped_ptr.h"
#include "ui/aura/root_window.h"
#include "ui/gfx/transform.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "xwalk/runtime/browser/ui/tizen_system_indicator.h"
#include "xwalk/runtime/browser/ui/top_view_layout_views.h"

namespace xwalk {

NativeAppWindowTizen::NativeAppWindowTizen(
    const NativeAppWindow::CreateParams& create_params)
    : NativeAppWindowViews(create_params) {
  if (SensorProvider::GetInstance())
    SensorProvider::GetInstance()->AddObserver(this);
}

NativeAppWindowTizen::~NativeAppWindowTizen() {
  if (SensorProvider::GetInstance())
    SensorProvider::GetInstance()->RemoveObserver(this);
}

void NativeAppWindowTizen::ViewHierarchyChanged(
    const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this) {
    NativeAppWindowViews::ViewHierarchyChangedDetails(details);
    scoped_ptr<TizenSystemIndicator> indicator(new TizenSystemIndicator());
    if (indicator->IsConnected()) {
      layout()->set_top_view(indicator.get());
      AddChildView(indicator.release());
    } else {
      LOG(WARNING) << "Failed to load indicator";
    }
  }
}

namespace {

gfx::Transform GetRotationAroundCenter(const gfx::Size& size,
                                       const gfx::Display::Rotation& rotation) {
  gfx::Transform transform;
  switch (rotation) {
    case gfx::Display::ROTATE_0:
      break;
    case gfx::Display::ROTATE_90:
      transform.Translate(size.width() - 1, 0);
      transform.Rotate(90);
      break;
    case gfx::Display::ROTATE_180:
      transform.Translate(size.width() - 1, size.height() - 1);
      transform.Rotate(180);
      break;
    case gfx::Display::ROTATE_270:
      transform.Translate(0, size.height() - 1);
      transform.Rotate(270);
      break;
    default:
      NOTREACHED();
  }
  return transform;
}

}  // namespace

void NativeAppWindowTizen::OnRotationChanged(gfx::Display::Rotation rotation) {
  aura::RootWindow* root = GetNativeWindow()->GetRootWindow();
  if (!root)
    return;

  // The size of the root window remains the same, we rotate and
  // possibly resize the contents of the window.
  gfx::Size size = GetBounds().size();
  root->SetTransform(GetRotationAroundCenter(size, rotation));

  gfx::Size content_size;
  if (rotation == gfx::Display::ROTATE_90
      || rotation == gfx::Display::ROTATE_270) {
    content_size = gfx::Size(size.height(), size.width());
  } else {
    content_size = size;
  }
  GetWidget()->GetRootView()->SetSize(content_size);
}

}  // namespace xwalk
