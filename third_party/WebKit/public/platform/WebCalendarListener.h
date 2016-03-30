// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCalendarListener_h
#define WebCalendarListener_h

#include "WebCalendarStatus.h"

namespace blink {

class WebCalendarStatus;

class WebCalendarListener {
public:
    virtual void resultCalendar(const WebCalendarStatus&) = 0;

    virtual ~WebCalendarListener() { }
};

} // namespace blink

#endif // WebCalendarListener_h
