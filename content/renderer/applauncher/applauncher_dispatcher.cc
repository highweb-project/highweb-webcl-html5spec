// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/applauncher/applauncher_dispatcher.h"

#include "content/public/common/service_registry.h"
#include "content/public/renderer/render_thread.h"

#include "third_party/WebKit/public/platform/WebString.h"

namespace content {

AppLauncherDispatcher::AppLauncherDispatcher() {
	if (ServiceRegistry* registry = RenderThread::Get()->GetServiceRegistry()) {
		registry->ConnectToRemoteService(mojo::GetProxy(&manager_));
	}
}

AppLauncherDispatcher::~AppLauncherDispatcher() {
	manager_.reset();
	listener_ = nullptr;
	if (webapp_status.applist != nullptr) {
		delete[] webapp_status.applist;
		webapp_status.applist = nullptr;
	}
}

void AppLauncherDispatcher::launchApp(const blink::WebString& packageName, 
	const blink::WebString& activityName, blink::WebAppLauncherListener* mCallback) {
	listener_ = mCallback;
	manager_->LaunchApp(mojo::String(packageName.utf8()), mojo::String(activityName.utf8()),
		base::Bind(&AppLauncherDispatcher::resultCodeCallback, base::Unretained(this)));
}

void AppLauncherDispatcher::removeApp(const blink::WebString& packageName, 
	blink::WebAppLauncherListener* mCallback) {
	listener_ = mCallback;
	manager_->RemoveApp(mojo::String(packageName.utf8()), 
		base::Bind(&AppLauncherDispatcher::resultCodeCallback, base::Unretained(this)));
}

void AppLauncherDispatcher::getAppList(const blink::WebString& mimeType, 
	blink::WebAppLauncherListener* mCallback) {
	listener_ = mCallback;
	manager_->GetAppList(mojo::String(mimeType.utf8()), 
		base::Bind(&AppLauncherDispatcher::resultCodeCallback, base::Unretained(this)));
}

void AppLauncherDispatcher::getApplicationInfo(const blink::WebString& packageName, 
	const int flags, blink::WebAppLauncherListener* mCallback) {
	listener_ = mCallback;
	manager_->GetApplicationInfo(mojo::String(packageName.utf8()), flags,
		base::Bind(&AppLauncherDispatcher::resultCodeCallback, base::Unretained(this)));
}

void AppLauncherDispatcher::resultCodeCallback(
		device::AppLauncher_ResultCodePtr resultCodePtr) {
	DCHECK(resultCodePtr);
	
	if (resultCodePtr != nullptr) {
		webapp_status.functionCode = resultCodePtr->functionCode;
		switch(resultCodePtr->functionCode) {
			case blink::WebAppStatus::function::FUNC_LAUNCH_APP:
			case blink::WebAppStatus::function::FUNC_REMOVE_APP:
			{
				webapp_status.resultCode = resultCodePtr->resultCode;
				break;
			}
			case blink::WebAppStatus::function::FUNC_GET_APP_LIST:
			case blink::WebAppStatus::function::FUNC_GET_APPLICATION_INFO:
			{
				webapp_status.resultCode = resultCodePtr->resultCode;
				if (webapp_status.resultCode != blink::WebAppStatus::status::SUCCESS){
					break;
				}
				unsigned listSize = resultCodePtr->applist.size();
				webapp_status.applist = new blink::WebApplicationInfo[listSize]();
				webapp_status.infoListSize = listSize;
				for(unsigned i = 0; i < listSize; i++) {
					webapp_status.applist[i].name = blink::WebString::fromUTF8(resultCodePtr->applist[i]->name.get());
					webapp_status.applist[i].className = blink::WebString::fromUTF8(resultCodePtr->applist[i]->className.get());
					webapp_status.applist[i].dataDir = blink::WebString::fromUTF8(resultCodePtr->applist[i]->dataDir.get());
					webapp_status.applist[i].enabled = resultCodePtr->applist[i]->enabled;
					webapp_status.applist[i].flags = resultCodePtr->applist[i]->flags;
					webapp_status.applist[i].permission = blink::WebString::fromUTF8(resultCodePtr->applist[i]->permission.get());
					webapp_status.applist[i].processName = blink::WebString::fromUTF8(resultCodePtr->applist[i]->processName.get());
					webapp_status.applist[i].targetSdkVersion = resultCodePtr->applist[i]->targetSdkVersion;
					webapp_status.applist[i].theme = resultCodePtr->applist[i]->theme;
					webapp_status.applist[i].uid = resultCodePtr->applist[i]->uid;
					webapp_status.applist[i].packageName = blink::WebString::fromUTF8(resultCodePtr->applist[i]->packageName.get());
				}
				break;
			}
		}
	}
	if (listener_ != nullptr) {
		listener_->resultAppLauncher(webapp_status);
	}
}

}  // namespace content
