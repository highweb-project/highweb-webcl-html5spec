// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AppLauncherListener_h
#define AppLauncherListener_h

#include "wtf/text/WTFString.h"
#include "public/platform/WebAppLauncherListener.h"

namespace blink {

class AppStatus;
class AppLauncher;

class AppLauncherListener final : public GarbageCollectedFinalized<AppLauncherListener>, public WebAppLauncherListener {
public:
	static AppLauncherListener* instance(AppLauncher*);
	~AppLauncherListener();

	void resultAppLauncher(const WebAppStatus&) override;

	AppStatus* lastData();

	DECLARE_VIRTUAL_TRACE();

private:
	AppLauncherListener(AppLauncher*);
	Member<AppStatus> mAppStatus;
	Member<AppLauncher> mAppLauncher;
};

} // namespace blink

#endif // AppLauncherListener_h