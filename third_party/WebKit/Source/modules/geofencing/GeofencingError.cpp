// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/geofencing/GeofencingError.h"

#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

DOMException* GeofencingError::take(ScriptPromiseResolver*, const WebGeofencingError& webError)
{
    switch (webError.errorType) {
    case WebGeofencingError::ErrorTypeAbort:
        return DOMException::create(AbortError, webError.message);
    case WebGeofencingError::ErrorTypeUnknown:
        return DOMException::create(UnknownError, webError.message);
    }
    ASSERT_NOT_REACHED();
    return DOMException::create(UnknownError);
}

} // namespace blink
