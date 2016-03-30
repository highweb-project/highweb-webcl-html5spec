/*
 * DeviceApiPermissionController.cpp
 *
 *  Created on: 2015. 12. 1.
 *      Author: azureskybox
 */
#include "config.h"
#include "modules/device_api/DeviceApiPermissionController.h"
#include "core/dom/Document.h"

namespace blink {
DeviceApiPermissionController::~DeviceApiPermissionController()
{
}

void DeviceApiPermissionController::provideTo(LocalFrame& frame, WebDeviceApiPermissionCheckClient* client)
{
	DeviceApiPermissionController* controller = new DeviceApiPermissionController(frame, client);
    WillBeHeapSupplement<LocalFrame>::provideTo(frame, supplementName(), adoptPtrWillBeNoop(controller));
}

DeviceApiPermissionController* DeviceApiPermissionController::from(LocalFrame& frame)
{
    return static_cast<DeviceApiPermissionController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName()));
}

DeviceApiPermissionController::DeviceApiPermissionController(LocalFrame& frame, WebDeviceApiPermissionCheckClient* client)
    : m_client(client)
{
}

const char* DeviceApiPermissionController::supplementName()
{
    return "DeviceApiPermissionController";
}

DEFINE_TRACE(DeviceApiPermissionController)
{
    WillBeHeapSupplement<LocalFrame>::trace(visitor);
}

}



