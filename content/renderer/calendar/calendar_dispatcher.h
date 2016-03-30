// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_CALENDAR_DISPATCHER_H_
#define CONTENT_RENDERER_CALENDAR_DISPATCHER_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/common/content_export.h"
#include "device/calendar/calendar_ResultCode.mojom.h"
#include "device/calendar/calendar_manager.mojom.h"

#include "third_party/WebKit/public/platform/WebCalendarListener.h"
#include "third_party/WebKit/public/platform/WebCalendarStatus.h"

namespace blink {
class WebCalendarListener;
class WebString;
class WebCalendarStatus;
}

namespace content {

class CONTENT_EXPORT CalendarDispatcher {
	public:
		explicit CalendarDispatcher();
		~CalendarDispatcher();
		void resultCodeCallback(device::Calendar_ResultCodePtr resultCode);

		void calendarDeviceApiFindEvent(const blink::WebString& startBefore, const blink::WebString& startAfter, const blink::WebString& endBefore, const blink::WebString& endAfter, bool multiple, blink::WebCalendarListener* callback);
		void calendarDeviceApiAddEvent(blink::WebCalendarInfo* event, blink::WebCalendarListener* callback);
		void calendarDeviceApiUpdateEvent(blink::WebCalendarInfo* event, blink::WebCalendarListener* callback);
		void calendarDeviceApiDeleteEvent(const blink::WebString& id, blink::WebCalendarListener* callback);

	private:
		device::CalendarManagerPtr manager_;
		blink::WebCalendarListener* listener_;
		blink::WebCalendarStatus webcalendar_status;

		DISALLOW_COPY_AND_ASSIGN(CalendarDispatcher);
};

}  // namespace content

#endif  // CONTENT_RENDERER_CALENDAR_DISPATCHER_H_
