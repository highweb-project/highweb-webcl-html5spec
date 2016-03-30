// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*
 * WebAppLauncherListener.h
 *
 *  Created on: 2015. 12. 2.
 *      Author: pjs3232
 */


#ifndef WebAppLauncherListener_h
#define WebAppLauncherListener_h

#include "WebAppStatus.h"

namespace blink {

class WebAppStatus;

class WebAppLauncherListener {
public:
    virtual void resultAppLauncher(const WebAppStatus&) = 0;

    virtual ~WebAppLauncherListener() { }
};

} // namespace blink

#endif // WebAppLauncherListener_h
