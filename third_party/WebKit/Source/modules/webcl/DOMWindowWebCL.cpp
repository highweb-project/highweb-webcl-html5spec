// Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
// Copyright (C) 2015 Intel Corporation All rights reserved.
// Copyright (C) 2016 INFRAWARE, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webcl/DOMWindowWebCL.h"

#include "core/frame/LocalDOMWindow.h"
#include "modules/webcl/WebCL.h"

namespace blink {

DOMWindowWebCL::DOMWindowWebCL(LocalDOMWindow& window)
    : DOMWindowProperty(window.frame())
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowWebCL);

const char* DOMWindowWebCL::supplementName()
{
    return "DOMWindowWebCL";
}

DOMWindowWebCL& DOMWindowWebCL::from(LocalDOMWindow& window)
{
	DOMWindowWebCL* supplement = static_cast<DOMWindowWebCL*>(WillBeHeapSupplement<LocalDOMWindow>::from(window, supplementName()));
    if (!supplement) {
        supplement = new DOMWindowWebCL(window);
        provideTo(window, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

WebCL* DOMWindowWebCL::webcl(DOMWindow& window)
{
	return DOMWindowWebCL::from(toLocalDOMWindow(window)).webclCreate(window.executionContext());
}

WebCL* DOMWindowWebCL::webclCreate(ExecutionContext* context) const
{
    if (!m_webcl && frame())
    	m_webcl = WebCL::create(context);
    return m_webcl.get();
}

DEFINE_TRACE(DOMWindowWebCL)
{
	visitor->trace(m_webcl);
	WillBeHeapSupplement<LocalDOMWindow>::trace(visitor);
	DOMWindowProperty::trace(visitor);
}

} // namespace blink
