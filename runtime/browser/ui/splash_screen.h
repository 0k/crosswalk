// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_RUNTIME_BROWSER_UI_SPLASH_SCREEN_H_
#define XWALK_RUNTIME_BROWSER_UI_SPLASH_SCREEN_H_

#include "base/files/file_path.h"
#include "content/public/browser/web_contents_observer.h"
#include "ui/compositor/layer_animation_observer.h"

namespace content {
class WebContents;
class RenderViewHost;
}

namespace ui {
class Layer;
}

namespace views {
class Widget;
}

namespace xwalk {

class SplashScreen : public content::WebContentsObserver,
                     public ui::ImplicitAnimationObserver {
 public:
  SplashScreen(views::Widget* host,
               const base::FilePath& file,
               content::WebContents* web_contents);
  ~SplashScreen();

  void Start();
  void Stop();

  // Overridden from content::WebContentsObserver.
  virtual void DidFinishLoad(int64 frame_id,
                             const GURL& validated_url,
                             bool is_main_frame,
                             content::RenderViewHost* render_view_host)
                             OVERRIDE;

  virtual void DidFailLoad(int64 frame_id,
                           const GURL& validated_url,
                           bool is_main_frame,
                           int error_code,
                           const base::string16& error_description,
                           content::RenderViewHost* render_view_host) OVERRIDE;

  // ui::ImplicitAnimationObserver overrides:
  virtual void OnImplicitAnimationsCompleted() OVERRIDE;

 private:
  views::Widget* widget_host_;
  base::FilePath splash_screen_image_;

  scoped_ptr<ui::Layer> layer_;
  class SplashScreenLayerDelegate;
  scoped_ptr<SplashScreenLayerDelegate> layer_delegate_;

  bool is_started;

  DISALLOW_COPY_AND_ASSIGN(SplashScreen);
};

}  // namespace xwalk

#endif  // XWALK_RUNTIME_BROWSER_UI_SPLASH_SCREEN_H_
