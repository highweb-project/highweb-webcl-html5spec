// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "AppLauncherListener.h"
#include "AppStatus.h"
#include "public/platform/Platform.h"
#include "AppLauncher.h"
#include "modules/applauncher/ApplicationInfo.h"
#include "wtf/text/WTFString.h"

namespace blink {

AppLauncherListener* AppLauncherListener::instance(AppLauncher* launcher)
{
	AppLauncherListener* callback = new AppLauncherListener(launcher);
	return callback;
}

AppLauncherListener::AppLauncherListener(AppLauncher* launcher)
{
	mAppLauncher = launcher;
}

AppLauncherListener::~AppLauncherListener()
{
	mAppStatus.get()->appList().clear();
	mAppStatus = NULL;
	mAppLauncher = NULL;
}

DEFINE_TRACE(AppLauncherListener)
{
	visitor->trace(mAppStatus);
	visitor->trace(mAppLauncher);
}

void AppLauncherListener::resultAppLauncher(const WebAppStatus& appStatus)
{
	mAppStatus = AppStatus::create();
	mAppStatus.get()->setResultCode(appStatus.resultCode);
	switch(appStatus.functionCode) {
		case WebAppStatus::function::FUNC_GET_APP_LIST: {
			if (appStatus.applist != nullptr) {
				unsigned size = appStatus.infoListSize;
				for(unsigned i = 0; i < size; i++) {
					ApplicationInfo info = ApplicationInfo();
					info.setName(WTF::String(appStatus.applist[i].name));
					info.setClassName(WTF::String(appStatus.applist[i].className));
					info.setDataDir(WTF::String(appStatus.applist[i].dataDir));
					info.setEnabled(appStatus.applist[i].enabled);
					info.setFlags(appStatus.applist[i].flags);
					info.setPermission(WTF::String(appStatus.applist[i].permission));
					info.setProcessName(WTF::String(appStatus.applist[i].processName));
					info.setTargetSdkVersion(appStatus.applist[i].targetSdkVersion);
					info.setTheme(appStatus.applist[i].theme);
					info.setUid(appStatus.applist[i].uid);
					info.setPackageName(appStatus.applist[i].packageName);
					mAppStatus.get()->appList().append(info);
				}
			}
			break;
		}
		case WebAppStatus::function::FUNC_GET_APPLICATION_INFO: {
			if (appStatus.applist != nullptr) {
				ApplicationInfo info = ApplicationInfo();
				info.setName(WTF::String(appStatus.applist[0].name));
				info.setClassName(WTF::String(appStatus.applist[0].className));
				info.setDataDir(WTF::String(appStatus.applist[0].dataDir));
				info.setEnabled(appStatus.applist[0].enabled);
				info.setFlags(appStatus.applist[0].flags);
				info.setPermission(WTF::String(appStatus.applist[0].permission));
				info.setProcessName(WTF::String(appStatus.applist[0].processName));
				info.setTargetSdkVersion(appStatus.applist[0].targetSdkVersion);
				info.setTheme(appStatus.applist[0].theme);
				info.setUid(appStatus.applist[0].uid);
				info.setPackageName(appStatus.applist[0].packageName);
				mAppStatus.get()->setAppInfo(info);
			}
			break;
		}
	}
	mAppLauncher->resultCodeCallback();
}

AppStatus* AppLauncherListener::lastData()
{
	if (mAppStatus != nullptr)
		return mAppStatus.get();
	else
		return nullptr;
}

} // namespace blink