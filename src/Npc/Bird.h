#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class Nerve;
}

class Bird : public al::LiveActor {
public:
    Bird(const char* name);
    void init(const al::ActorInitInfo& info) override;
    void initAfterPlacement() override;
    void initGlideOn(const sead::Matrix34f*, const sead::Vector3f&);
    void initGlideOff(const sead::Matrix34f*, const sead::Vector3f&, bool);
    void makeActorAlive() override;
    void appear() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    void control() override;
    void updateCollider() override;
    void startFlyAwayHomeLanding();
    void startFlyAwayKoopa();
    void startFlyAwayKoopa(const sead::Vector3f&);
    void tryStartFlyAwayTRex(const sead::Vector3f&);
    void exeFreeze();
    void exeDelay();
    void exeWait();
    void endWait();
    void exeFlyRail();
    void exeFlyRailGoal();
    void exeFlyAway();
    void endFlyAway();
    void exeReset();
    void exeGatheringFallDown();
    void endGatheringFallDown();
    void exeGatheringFallDownSlow();
    void exeGatheringLand();
    void createForKoopaLv1(const al::ActorInitInfo&);
    void createForGatheringSpot(const char*, const al::ActorInitInfo&, const char*);
    void appearGatheringSpot(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);
    void forceFlyAwayGatheringSpot();
    bool isWait() const;
    static Bird* createBirdGlideDownUfo(const char*, const al::ActorInitInfo&);
    void tryStartGlideDown();
    void tryStartFlyAway();

private:
    struct SomeStruct {
        f32 _0x0 = 1000;
        s32 _0x4 = -1;
        bool _0x8 = false;
        s32 _0xc = 0;
    };

    SomeStruct* _0x108;
    s64 _0x110 = 0;
    s64 _0x118 = 0;
    s64 _0x120 = 0;
    s64 _0x128 = 0;
    s64 _0x130 = 0;
    s64 _0x138 = 0;
    sead::Quatf _0x140 = sead::Quatf::unit;
    s64 _0x150 = 0;
    const al::Nerve* _0x158;
    sead::Vector3f _0x160 = {0, 0, 0};
    s32 _0x16c = 0;
    s64 _0x170 = 0;
    s64 _0x178;
    s64 _0x180;
    s64 _0x188;
    s64 _0x190 = 0;
    s64 _0x198 = 0;
    s64 _0x1a0 = 0;
    s64 _0x1a8 = 0;
    s32 _0x1b0 = 0;
    s16 _0x1b4 = 0;
    s8 _0x1b6 = 0;
    s64 _0x1b8 = 0;
    s64 _0x1c0 = 0;
    s32 _0x1c8 = 0;
    s8 _0x1cc = 0;
};

static_assert(sizeof(Bird) == 0x1d0);
