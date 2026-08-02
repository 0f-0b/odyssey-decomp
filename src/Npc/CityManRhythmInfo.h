#pragma once

#include <basis/seadTypes.h>

namespace al {
class LiveActor;
}

class CityManRhythmInfo {
public:
    struct CurAnimInfo;
    CityManRhythmInfo(al::LiveActor*, const u8*, bool, f32);
    void initAnimInfo(const u8*);
    void update(bool);
    bool isLooping();
    void resetRhythmInfo(f32);
    void updateAnim();
    void checkSameBeatAnimInfo(CurAnimInfo&, s32);
    s32 getAnimId(s32);
    f32 getAnimBeat(s32);

    f32 get_0xc() const { return _0xc; }

    s32 get_0x24() const { return _0x24; }

    s32 get_0x30() const { return _0x30; }

    bool get_0x35() const { return _0x35; }

private:
    void* _padding_0x0;
    s32 _0x8;
    f32 _0xc;
    void* _padding_0x10[0x2];
    s32 _0x20;
    s32 _0x24;
    void* _padding_0x28;
    s32 _0x30;
    bool _0x34;
    bool _0x35;
};

static_assert(sizeof(CityManRhythmInfo) == 0x38);
