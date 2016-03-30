// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "config.h"
#include "CalendarStatus.h"

namespace blink {

void CalendarStatus::setResultCode(int code) {
	mResultCode = code;
}

int CalendarStatus::resultCode() {
	return mResultCode;
}

HeapVector<CalendarInfo>& CalendarStatus::calendarList() {
	return mCalendarList;
}

CalendarStatus::~CalendarStatus() {
	mCalendarList.clear();
}

CalendarStatus::CalendarStatus() {
}

void CalendarStatus::setCalendarInfo(CalendarInfo& info) {
	mCalendarInfo = info;
}

void CalendarStatus::calendarInfo(CalendarInfo& info) {
	info = mCalendarInfo;
}

} // namespace blink
