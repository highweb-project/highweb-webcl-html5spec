// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "config.h"
#include "AppStatus.h"

namespace blink {

	void AppStatus::setResultCode(int code) {
		mResultCode = code;
	}

	int AppStatus::resultCode() {
		return mResultCode;
	}

	HeapVector<ApplicationInfo>& AppStatus::appList() {
		return mAppList;
	}

	AppStatus::~AppStatus() {
		mAppList.clear();
	}

	AppStatus::AppStatus() {
	}

	void AppStatus::setAppInfo(ApplicationInfo& info) {
		mAppInfo = info;
	}

	void AppStatus::appInfo(ApplicationInfo& info) {
		info = mAppInfo;
	}

} // namespace blink