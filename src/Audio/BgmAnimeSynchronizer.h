#pragma once

#include <basis/seadTypes.h>

namespace al {
class ByamlIter;
class LiveActor;
}  // namespace al
class BgmSyncTargetActionInfo;

class BgmAnimeSynchronizer {
public:
    static void isCreatable(const al::LiveActor*);
    static BgmAnimeSynchronizer* tryCreate(al::LiveActor*, al::ByamlIter&);
    void registSyncTargetActionInfo(BgmSyncTargetActionInfo*);
    BgmAnimeSynchronizer(al::LiveActor*, const char*, BgmSyncTargetActionInfo*);
    void init();
    BgmAnimeSynchronizer(al::LiveActor*, const char*, s32);
    void trySyncBgm();
    void setSyncChaseRateOffsetMax(f32, f32, f32, f32);

private:
    void* _padding[0xa];
};

static_assert(sizeof(BgmAnimeSynchronizer) == 0x50);
