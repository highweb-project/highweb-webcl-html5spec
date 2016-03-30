// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceCpu_h
#define DeviceCpu_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"
#include "wtf/ListHashSet.h"
#include "core/dom/Document.h"
#include "modules/EventTargetModules.h"

#include "public/platform/modules/device_api/WebDeviceApiPermissionCheckClient.h"

namespace blink {

class LocalFrame;
class DeviceCpuListener;
class DeviceCpuScriptCallback;
class DeviceCpuStatus;

class DeviceCpu final : public RefCountedGarbageCollectedEventTargetWithInlineData<DeviceCpu>, public ActiveDOMObject{
	REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(DeviceCpu);
	DEFINE_WRAPPERTYPEINFO();
	WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DeviceCpu);
// public:
	// static BatteryManager* create(ExecutionContext*);
	// ~BatteryManager() override;
	// : public GarbageCollected<DeviceCpu>
	// , public ScriptWrappable {
	// DEFINE_WRAPPERTYPEINFO();
public:

	enum ErrorCodeList{
		// Exception code
		SUCCESS = 0,
		FAILURE = -1,
		NOT_ENABLED_PERMISSION = -2,
	};

	enum function {
		FUNC_GET_CPU_LOAD = 0,
	};

	struct functionData {
		int functionCode = -1;
		DeviceCpuScriptCallback* scriptCallback = nullptr;
		functionData(int code) {
			functionCode = code;
		}
	};

	static DeviceCpu* create(Document& document) {
		DeviceCpu* devicecpu = new DeviceCpu(document);
		devicecpu->suspendIfNeeded();
		return devicecpu;
	}
	~DeviceCpu() override;

	void load(DeviceCpuScriptCallback* callback);
	void resultCodeCallback();
	void notifyCallback(DeviceCpuStatus*, DeviceCpuScriptCallback*);
	void notifyError(int, DeviceCpuScriptCallback*);
	void continueFunction();

	void requestPermission();
	void onPermissionChecked(PermissionResult result);

	const WTF::AtomicString& interfaceName() const override { return EventTargetNames::DeviceCpu; }
	ExecutionContext* executionContext() const override { return ContextLifecycleObserver::executionContext(); }

	// ActiveDOMObject implementation.
	void suspend() override;
	void resume() override;
	void stop() override;
	bool hasPendingActivity() const override;

	DECLARE_TRACE();

private:
	DeviceCpu(Document& document);

	Member<DeviceCpuListener> mCallback = nullptr;

	Deque<functionData*> d_functionData;

	ListHashSet<DeviceCpuScriptCallback*> callbackList;

	WTF::String mOrigin;
	WebDeviceApiPermissionCheckClient* mClient;

	// Document* document;

	bool ViewPermissionState = false;
	bool isPending = false;
};

} // namespace blink

#endif // DeviceCpu_h