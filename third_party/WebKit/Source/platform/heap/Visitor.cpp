// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/heap/Visitor.h"

#include "platform/heap/BlinkGC.h"
#include "platform/heap/MarkingVisitor.h"
#include "platform/heap/ThreadState.h"

namespace blink {

PassOwnPtr<Visitor> Visitor::create(ThreadState* state, BlinkGC::GCType gcType)
{
    switch (gcType) {
    case BlinkGC::GCWithSweep:
    case BlinkGC::GCWithoutSweep:
        return adoptPtr(new MarkingVisitor<Visitor::GlobalMarking>(state));
    case BlinkGC::TakeSnapshot:
        return adoptPtr(new MarkingVisitor<Visitor::SnapshotMarking>(state));
    case BlinkGC::ThreadTerminationGC:
        return adoptPtr(new MarkingVisitor<Visitor::ThreadLocalMarking>(state));
    case BlinkGC::ThreadLocalWeakProcessing:
        return adoptPtr(new MarkingVisitor<Visitor::WeakProcessing>(state));
    default:
        ASSERT_NOT_REACHED();
    }
    return nullptr;
}

Visitor::Visitor(ThreadState* state, MarkingMode markingMode)
    : m_state(state)
    , m_markingMode(markingMode)
{
    // See ThreadState::runScheduledGC() why we need to already be in a
    // GCForbiddenScope before any safe point is entered.
    m_state->enterGCForbiddenScope();

    ASSERT(m_state->checkThread());
}

Visitor::~Visitor()
{
    m_state->leaveGCForbiddenScope();
}

} // namespace blink
