// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/calendar/calendar_dispatcher.h"

#include "content/public/common/service_registry.h"
#include "content/public/renderer/render_thread.h"

#include "third_party/WebKit/public/platform/WebString.h"

namespace content {

CalendarDispatcher::CalendarDispatcher() {
	DLOG(INFO) << "CalendarDispatcher, manager_ : " << manager_;
	if (ServiceRegistry* registry = RenderThread::Get()->GetServiceRegistry()) {
		registry->ConnectToRemoteService(mojo::GetProxy(&manager_));
		DLOG(INFO) << "CalendarDispatcher, ConnectToRemoteService, manager_ : " << manager_;
	}
}

CalendarDispatcher::~CalendarDispatcher() {
	manager_.reset();
	listener_ = nullptr;
	if (webcalendar_status.calendarlist != nullptr) {
		webcalendar_status.calendarlist = nullptr;
	}
}

void CalendarDispatcher::calendarDeviceApiAddEvent(blink::WebCalendarInfo* event, blink::WebCalendarListener* callback) {
	DLOG(INFO) << "CalendarDispatcher::calendarDeviceApiAddEvent, manager_ : " << manager_;

	device::Calendar_CalendarInfoPtr ptr(device::Calendar_CalendarInfo::New());

	ptr.get()->id = event->id.utf8();
	ptr.get()->description = event->description.utf8();
	ptr.get()->location = event->location.utf8();
	ptr.get()->summary = event->summary.utf8();
	ptr.get()->start = event->start.utf8();
	ptr.get()->end = event->end.utf8();
	ptr.get()->status = event->status.utf8();
	ptr.get()->transparency = event->transparency.utf8();
	ptr.get()->reminder = event->reminder.utf8();

	listener_ = callback;
	manager_->CalendarDeviceApiAddEvent(ptr.Pass(), base::Bind(&CalendarDispatcher::resultCodeCallback, base::Unretained(this)));
}

void CalendarDispatcher::calendarDeviceApiUpdateEvent(blink::WebCalendarInfo* event, blink::WebCalendarListener* callback) {
	DLOG(INFO) << "CalendarDispatcher::calendarDeviceApiUpdateEvent, manager_ : " << manager_;

	device::Calendar_CalendarInfoPtr ptr(device::Calendar_CalendarInfo::New());

	ptr.get()->id = event->id.utf8();
	ptr.get()->description = event->description.utf8();
	ptr.get()->location = event->location.utf8();
	ptr.get()->summary = event->summary.utf8();
	ptr.get()->start = event->start.utf8();
	ptr.get()->end = event->end.utf8();
	ptr.get()->status = event->status.utf8();
	ptr.get()->transparency = event->transparency.utf8();
	ptr.get()->reminder = event->reminder.utf8();

	listener_ = callback;
	manager_->CalendarDeviceApiUpdateEvent(ptr.Pass(), base::Bind(&CalendarDispatcher::resultCodeCallback, base::Unretained(this)));
}

void CalendarDispatcher::calendarDeviceApiFindEvent(const blink::WebString& startBefore, const blink::WebString& startAfter, const blink::WebString& endBefore, const blink::WebString& endAfter, bool multiple, blink::WebCalendarListener* callback) {
	DLOG(INFO) << "CalendarDispatcher::calendarDeviceApiFindEvent, manager_ : " << manager_;

	listener_ = callback;
	manager_->CalendarDeviceApiFindEvent(mojo::String(startBefore.utf8()), mojo::String(startAfter.utf8()), mojo::String(endBefore.utf8()), mojo::String(endAfter.utf8()), multiple, base::Bind(&CalendarDispatcher::resultCodeCallback, base::Unretained(this)));
}

void CalendarDispatcher::calendarDeviceApiDeleteEvent(const blink::WebString& id, blink::WebCalendarListener* callback) {
	DLOG(INFO) << "CalendarDispatcher::calendarDeviceApiDeleteEvent, manager_ : " << manager_;

	listener_ = callback;
	manager_->CalendarDeviceApiDeleteEvent(mojo::String(id.utf8()), base::Bind(&CalendarDispatcher::resultCodeCallback, base::Unretained(this)));
}

void CalendarDispatcher::resultCodeCallback(
		device::Calendar_ResultCodePtr resultCodePtr) {
	DCHECK(resultCodePtr);
	DLOG(INFO) << "CalendarDispatcher::resultCodeCallback";

	if (resultCodePtr != nullptr) {
		webcalendar_status.functionCode = resultCodePtr->functionCode;
		webcalendar_status.resultCode = resultCodePtr->resultCode;
		switch(resultCodePtr->functionCode) {
			case 1:
			{
				unsigned listSize = resultCodePtr->calendarlist.size();
				DLOG(INFO) << "CalendarDispatcher::resultCodeCallback, listSize : " << listSize;
				webcalendar_status.calendarlist = new blink::WebCalendarInfo[listSize]();
				webcalendar_status.infoListSize = listSize;
				for(unsigned i = 0; i < listSize; i++) {
					webcalendar_status.calendarlist[i].id = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->id.get());
					webcalendar_status.calendarlist[i].description = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->description.get());
					webcalendar_status.calendarlist[i].location = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->location.get());
					webcalendar_status.calendarlist[i].summary = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->summary.get());
					webcalendar_status.calendarlist[i].start = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->start.get());
					webcalendar_status.calendarlist[i].end = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->end.get());
					webcalendar_status.calendarlist[i].status = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->status.get());
					webcalendar_status.calendarlist[i].transparency = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->transparency.get());
					webcalendar_status.calendarlist[i].reminder = blink::WebString::fromUTF8(resultCodePtr->calendarlist[i]->reminder.get());

					DLOG(INFO) << "resultCodePtr->calendarlist[" << i << "]->start.get() : " << webcalendar_status.calendarlist[i].start.utf8().data();
				}
			}
			break;
		}
	}

	if (listener_ != nullptr) {
		listener_->resultCalendar(webcalendar_status);
	}
}

}  // namespace content
