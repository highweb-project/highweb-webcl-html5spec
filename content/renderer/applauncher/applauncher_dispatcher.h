// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_APPLAUNCHER_DISPATCHER_H_
#define CONTENT_RENDERER_APPLAUNCHER_DISPATCHER_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/common/content_export.h"
#include "device/applauncher/applauncher_ResultCode.mojom.h"
#include "device/applauncher/applauncher_manager.mojom.h"

#include "third_party/WebKit/public/platform/WebAppLauncherListener.h"
#include "third_party/WebKit/public/platform/WebAppStatus.h"

namespace blink {
class WebAppLauncherListener;
class WebString;
class WebAppStatus;
}

namespace content {

class CONTENT_EXPORT AppLauncherDispatcher {
	public:
		explicit AppLauncherDispatcher();
		~AppLauncherDispatcher();
		void launchApp(const blink::WebString& packageName, const blink::WebString& activityName, blink::WebAppLauncherListener* mCallback);
		void removeApp(const blink::WebString& packageName, blink::WebAppLauncherListener* mCallback);
		void getAppList(const blink::WebString& mimeType, blink::WebAppLauncherListener* mCallback);
		void getApplicationInfo(const blink::WebString& packageName, const int flags, blink::WebAppLauncherListener* mCallback);
		void resultCodeCallback(device::AppLauncher_ResultCodePtr resultCode);
	private:
		device::AppLauncherManagerPtr manager_;
		blink::WebAppLauncherListener* listener_;
		blink::WebAppStatus webapp_status;

		DISALLOW_COPY_AND_ASSIGN(AppLauncherDispatcher);
};

}  // namespace content

#endif  // CONTENT_RENDERER_APPLAUNCHER_DISPATCHER_H_
