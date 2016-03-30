#ifndef SendAndroidBroadcastCallback_h
#define SendAndroidBroadcastCallback_h

#include "platform/heap/Handle.h"
#include "base/logging.h"

namespace blink {

class SendAndroidBroadcastCallback : public ThreadSafeRefCountedWillBeGarbageCollectedFinalized<SendAndroidBroadcastCallback> {
public:
    virtual ~SendAndroidBroadcastCallback() { DLOG(INFO) << "SendAndroidBroadcastCallback, Finalized..."; }
    DEFINE_INLINE_VIRTUAL_TRACE() { }
    virtual void onResult(const WTF::String& result) = 0;
};

}

#endif // SendAndroidBroadcastCallback_h
