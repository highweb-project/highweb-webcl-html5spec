// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*
 * WebAppStatus.h
 *
 *  Created on: 2015. 12. 2.
 *      Author: pjs3232
 */


#ifndef WebAppStatus_h
#define WebAppStatus_h

#include "WebString.h"

namespace blink {

struct WebApplicationInfo {
	WebString name;
	WebString className;
	WebString dataDir;
	bool enabled;
	int flags;
	WebString permission;
	WebString processName;
	int targetSdkVersion;
	int theme;
	int uid;
	WebString packageName;
};

class WebAppStatus {
public:
	enum status {
		// Exception code
		SUCCESS = 0,
		FAILURE = -1,
		NOT_INSTALLED_APP = -2,
		INVALID_PACKAGE_NAME = -3,
		NOT_ENABLED_PERMISSION = -4,
		INVALID_FLAGS = -5,
	};

	enum function {
		FUNC_LAUNCH_APP = 1,
		FUNC_REMOVE_APP,
		FUNC_GET_APP_LIST,
		FUNC_GET_APPLICATION_INFO,
	};

	WebAppStatus()
	{
		applist = nullptr;
	}

	unsigned functionCode = 0;
	int resultCode = -1;

	int infoListSize = 0;

	WebApplicationInfo* applist;
};

} // namespace blink

#endif // WebAppStatus_h
