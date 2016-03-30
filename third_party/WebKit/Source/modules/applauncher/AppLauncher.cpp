// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/applauncher/AppLauncher.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "core/frame/LocalFrame.h"
#include "core/dom/Document.h"
#include "public/platform/Platform.h"
#include "public/platform/WebString.h"
#include "AppLauncherListener.h"
#include "AppStatus.h"
#include "AppLauncherScriptCallback.h"
#include "modules/applauncher/ApplicationInfo.h"
#include "base/logging.h"

#include "modules/device_api/DeviceApiPermissionController.h"
#include "public/platform/modules/device_api/WebDeviceApiPermissionCheckRequest.h"

namespace blink {

AppLauncher::AppLauncher(LocalFrame& frame)
	: mClient(DeviceApiPermissionController::from(frame)->client())
{
	mOrigin = frame.document()->url().strippedForUseAsReferrer();
	mClient->SetOrigin(mOrigin.latin1().data());
	d_functionData.clear();
}

AppLauncher::~AppLauncher()
{
}

void AppLauncher::launchApp(const String packageName, const String activityName, AppLauncherScriptCallback* callback) {
	functionData* data = new functionData(function::FUNC_LAUNCH_APP);
	data->scriptCallback = callback;
	data->str1 = packageName;
	data->str2 = activityName;
	d_functionData.append(data);
	data = nullptr;

	if (packageName.isEmpty()) {
		notifyError(code_list::INVALID_PACKAGE_NAME, callback);
		return;
	}
	if (d_functionData.size() == 1) {
		launchApp();
	}
}

void AppLauncher::launchApp() {
	if (!ViewPermissionState) {
		requestPermission(PermissionOptType::VIEW);
		return;
	}
	if (mCallback == NULL) {
		mCallback = AppLauncherListener::instance(this);
	}
	functionData* data = d_functionData.first();
	Platform::current()->launchApp(WebString(data->str1), WebString(data->str2), mCallback);
	data = nullptr;
}

void AppLauncher::removeApp(const String packageName, AppLauncherScriptCallback* callback) {
	functionData* data = new functionData(function::FUNC_REMOVE_APP);
	data->scriptCallback = callback;
	data->str1 = packageName;
	d_functionData.append(data);
	data = nullptr;

	if (packageName.isEmpty()) {
		notifyError(code_list::INVALID_PACKAGE_NAME, callback);
		return;
	}
	if (d_functionData.size() == 1) {
		removeApp();
	}
}

void AppLauncher::removeApp() {
	if (!DeletePermissionState) {
		requestPermission(PermissionOptType::DELETE);
		return;
	}
	if (mCallback == NULL) {
		mCallback = AppLauncherListener::instance(this);
	}
	functionData* data = d_functionData.first();
	Platform::current()->removeApp(WebString(data->str1), mCallback);
	data = nullptr;
}

void AppLauncher::getAppList(const String mimeType, AppLauncherScriptCallback* callback) {
	functionData* data = new functionData(function::FUNC_GET_APP_LIST);
	data->scriptCallback = callback;
	data->str1 = mimeType;
	d_functionData.append(data);
	data = nullptr;
	if (d_functionData.size() == 1) {
		getAppList();
	}
}

void AppLauncher::getAppList() {
	if (!ViewPermissionState) {
		requestPermission(PermissionOptType::VIEW);
		return;
	}
	if (mCallback == NULL) {
		mCallback = AppLauncherListener::instance(this);
	}
	functionData* data = d_functionData.first();
	Platform::current()->getAppList(WebString(data->str1), mCallback);
	data = nullptr;
}

void AppLauncher::getApplicationInfo(const String packageName, const int flags, AppLauncherScriptCallback* callback) {
	functionData* data = new functionData(function::FUNC_GET_APPLICATION_INFO);
	data->scriptCallback = callback;
	data->str1 = packageName;
	data->int1 = flags;
	d_functionData.append(data);
	data = nullptr;

	if (packageName.isEmpty()) {
		notifyError(code_list::INVALID_PACKAGE_NAME, callback);
		return;
	}
	if (flags != 0) {
		int checkFlag = flags & code_list::FLAG_GET_META_DATA;
		checkFlag = checkFlag & code_list::FLAG_SHARED_LIBRARY_FILES;
		checkFlag = checkFlag & code_list::FLAG_GET_UNINSTALLED_PACKAGES;
		if (checkFlag <= 0) {
			notifyError(code_list::INVALID_FLAGS, callback);
		}
	}
	if (d_functionData.size() == 1) {
		getApplicationInfo();
	}
}

void AppLauncher::getApplicationInfo() {
	if (!ViewPermissionState) {
		requestPermission(PermissionOptType::VIEW);
		return;
	}
	if (mCallback == NULL) {
		mCallback = AppLauncherListener::instance(this);
	}
	functionData* data = d_functionData.first();
	Platform::current()->getApplicationInfo(WebString(data->str1), data->int1, mCallback);
	data = nullptr;
}

void AppLauncher::resultCodeCallback() {
	AppStatus* status = nullptr;
	if (mCallback != nullptr) {
		status = mCallback->lastData();
		mCallback = nullptr;
	}
	if (status != nullptr) {
		notifyCallback(status, d_functionData.first()->scriptCallback);
	} else {
		notifyError(code_list::FAILURE, d_functionData.first()->scriptCallback);
	}
}

void AppLauncher::notifyCallback(AppStatus* status, AppLauncherScriptCallback* callback) {
	if (callback != NULL) {
		callback->handleEvent(status);
		callback = nullptr;
	}
	Platform::current()->resetAppLauncherDispatcher();
	if (d_functionData.size() > 0)
		d_functionData.removeFirst();
	continueFunction();
}

void AppLauncher::notifyError(int errorCode, AppLauncherScriptCallback* callback) {
	AppStatus* status = AppStatus::create();
	status->setResultCode(errorCode);
	notifyCallback(status, callback);
}

void AppLauncher::continueFunction() {
	if (d_functionData.size() > 0) {
		switch(d_functionData.first()->functionCode) {
			case function::FUNC_LAUNCH_APP: {
				launchApp();
				break;
			}
			case function::FUNC_REMOVE_APP: {
				removeApp();
				break;
			}
			case function::FUNC_GET_APP_LIST: {
				getAppList();
				break;
			}
			case function::FUNC_GET_APPLICATION_INFO: {
				getApplicationInfo();
				break;
			}
			default: {
				break;
			}
		}
	}
}

void AppLauncher::requestPermission(int operationType) {
	if(mClient) {
		switch(operationType) {
			case PermissionOptType::VIEW: {
				mClient->CheckPermission(new WebDeviceApiPermissionCheckRequest(
						PermissionAPIType::APPLICATION_LAUNCHER,
						PermissionOptType::VIEW,
						base::Bind(&AppLauncher::onPermissionChecked, base::Unretained(this))));
				break;
			}
			case PermissionOptType::DELETE: {
				mClient->CheckPermission(new WebDeviceApiPermissionCheckRequest(
						PermissionAPIType::APPLICATION_LAUNCHER,
						PermissionOptType::DELETE,
						base::Bind(&AppLauncher::onPermissionChecked, base::Unretained(this))));
				break;
			}
		}
	}
}

void AppLauncher::onPermissionChecked(PermissionResult result)
{
	if (result == WebDeviceApiPermissionCheckClient::DeviceApiPermissionRequestResult::RESULT_OK && d_functionData.size() > 0) {
		switch(d_functionData.first()->functionCode) {
			case function::FUNC_LAUNCH_APP:
			case function::FUNC_GET_APP_LIST:
			case function::FUNC_GET_APPLICATION_INFO: {
				ViewPermissionState = true;
				if (ViewPermissionState) {
					continueFunction();
					return;
				}
				break;
			}
			case function::FUNC_REMOVE_APP: {
				DeletePermissionState = true;
				if (DeletePermissionState) {
					continueFunction();
					return;
				}
				break;
			}
		}
	}
	notifyError(code_list::NOT_ENABLED_PERMISSION, d_functionData.first()->scriptCallback);
}

DEFINE_TRACE(AppLauncher)
{
	visitor->trace(mCallback);
	visitor->trace(d_functionData);
}

} // namespace blink
