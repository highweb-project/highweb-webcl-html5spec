// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCalendarStatus_h
#define WebCalendarStatus_h

#include "WebString.h"

namespace blink {

struct WebCalendarInfo {
	WebString id;
	WebString description;
	WebString location;
	WebString summary;
	WebString start;
	WebString end;
	WebString status;
	WebString transparency;
	WebString reminder;
};

class WebCalendarStatus {
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

	WebCalendarStatus()
	{
	}

	unsigned functionCode = 0;
	int resultCode = -1;

	int infoListSize = 0;

	WebCalendarInfo* calendarlist;
};

} // namespace blink

#endif // WebCalendarStatus_h
