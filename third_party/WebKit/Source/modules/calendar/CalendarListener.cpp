// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "CalendarListener.h"
#include "CalendarStatus.h"
#include "public/platform/Platform.h"
#include "Calendar.h"
#include "modules/calendar/CalendarInfo.h"
#include "wtf/text/WTFString.h"
#include "base/logging.h"

namespace blink {

CalendarListener* CalendarListener::instance(Calendar* calendar)
{
	CalendarListener* callback = new CalendarListener(calendar);
	return callback;
}

CalendarListener::CalendarListener(Calendar* calendar)
{
	mCalendar = calendar;
}

CalendarListener::~CalendarListener()
{
	mCalendarStatus.get()->calendarList().clear();
	mCalendarStatus = NULL;
	mCalendar = NULL;
}

DEFINE_TRACE(CalendarListener)
{
	visitor->trace(mCalendarStatus);
	visitor->trace(mCalendar);
}

void CalendarListener::resultCalendar(const WebCalendarStatus& calendarStatus)
{
	DLOG(INFO) << "CalendarListener::resultCalendar, calendarStatus.functionCode : " << calendarStatus.functionCode;

	mCalendarStatus = CalendarStatus::create();
	DLOG(INFO) << "mCalendarStatus : " << mCalendarStatus;
	mCalendarStatus.get()->setResultCode(calendarStatus.resultCode);
	switch(calendarStatus.functionCode) {
		case 1/*WebCalendarStatus::function::FUNC_GET_APP_LIST*/: {
			if (calendarStatus.calendarlist != nullptr) {
				unsigned size = calendarStatus.infoListSize;
				DLOG(INFO) << "CalendarListener::resultCalendar, size : " << size;
				for(unsigned i = 0; i < size; i++) {
					CalendarInfo info = CalendarInfo();
					info.setId(WTF::String(calendarStatus.calendarlist[i].id));
					info.setDescription(WTF::String(calendarStatus.calendarlist[i].description));
					info.setLocation(WTF::String(calendarStatus.calendarlist[i].location));
					info.setSummary(WTF::String(calendarStatus.calendarlist[i].summary));
					info.setStart(WTF::String(calendarStatus.calendarlist[i].start));
					info.setEnd(WTF::String(calendarStatus.calendarlist[i].end));
					info.setStatus(WTF::String(calendarStatus.calendarlist[i].status));
					info.setTransparency(WTF::String(calendarStatus.calendarlist[i].transparency));
					info.setReminder(WTF::String(calendarStatus.calendarlist[i].reminder));
					mCalendarStatus.get()->calendarList().append(info);

					DLOG(INFO) << "WTF::String(calendarStatus.calendarlist[i].start) : " << WTF::String(calendarStatus.calendarlist[i].start).utf8().data();
				}
			}
			break;
		}
	}
	mCalendar->resultCodeCallback();
}

CalendarStatus* CalendarListener::lastData()
{
	if (mCalendarStatus != nullptr)
		return mCalendarStatus.get();
	else
		return nullptr;
}

} // namespace blink
