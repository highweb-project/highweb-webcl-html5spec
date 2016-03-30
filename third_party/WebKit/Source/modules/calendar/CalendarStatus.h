// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef CalendarStatus_h
#define CalendarStatus_h

#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "public/platform/WebString.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/calendar/CalendarInfo.h"

namespace blink {

class CalendarInfo;

class CalendarStatus final : public GarbageCollectedFinalized<CalendarStatus>, public ScriptWrappable {
	DEFINE_WRAPPERTYPEINFO();
public:
	static CalendarStatus* create() {return new CalendarStatus();};
	~CalendarStatus();

	void setResultCode(int code);
	void setCalendarInfo(CalendarInfo& info);
	void calendarInfo(CalendarInfo& info);
	int resultCode();

	HeapVector<CalendarInfo>& calendarList();

	DEFINE_INLINE_TRACE() {visitor->trace(mCalendarList); };

private:
	CalendarStatus();

	int mResultCode = -1;

	HeapVector<CalendarInfo> mCalendarList;
	CalendarInfo mCalendarInfo;
};

} // namespace blink

#endif // CalendarStatus_h
