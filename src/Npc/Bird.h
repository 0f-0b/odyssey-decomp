#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class MtxConnector;
class Nerve;
}  // namespace al
class BirdStateFlyAwayBase;
class BirdStateWaitBase;
class ItemGenerator;
class WaterSurfaceShadow;

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
    void startFlyAwayKoopa(const sead::Vector3f& horizontal_dir);
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

    static Bird* createForKoopaLv1(const al::ActorInitInfo& info);
    static Bird* createForGatheringSpot(const char* name, const al::ActorInitInfo& info,
                                        const char*);
    void appearGatheringSpot(const sead::Vector3f& start_pos, const sead::Vector3f& destination,
                             const sead::Vector3f& horizontal_dir);
    void forceFlyAwayGatheringSpot();
    bool isWait() const;
    static Bird* createBirdGlideDownUfo(const char* name, const al::ActorInitInfo& info);
    void tryStartGlideDown();
    void tryStartFlyAway();

    struct Struct_0x108 {
        f32 playerWatchDistance = 1000;
        s32 waitStepAtRailEdge = -1;
        bool isInvalidTurnReverse = false;
        s32 flyAwayBySwitchDelayStep = 0;
    };

    static_assert(sizeof(Struct_0x108) == 0x10);

    struct HostAnimInfo {
        const al::CollisionParts* collisionParts = nullptr;
        sead::Vector3f initialTrans = {0, 0, 0};
        const char* sklAnimName = nullptr;
        s32 sklAnimTime = -1;
    };

    static_assert(sizeof(HostAnimInfo) == 0x28);

private:
    Struct_0x108* _0x108;
    ItemGenerator* mItemGenerator = nullptr;
    WaterSurfaceShadow* mWaterSurfaceShadow = nullptr;
    BirdStateWaitBase* mStateWait = nullptr;
    BirdStateFlyAwayBase* mStateFlyAway = nullptr;
    HostAnimInfo* mHostAnimInfo = nullptr;
    al::MtxConnector* mMtxConnector = nullptr;
    sead::Quatf _0x140 = sead::Quatf::unit;
    const char* mArchiveName = nullptr;
    const al::Nerve* mInitialNerve;
    sead::Vector3f mInitialTrans = {0, 0, 0};
    sead::Vector3f mInitialFront = {0, 0, 0};
    s64 _0x178;
    s64 _0x180;
    s64 _0x188;
    sead::Vector3f _0x190 = {0, 0, 0};
    sead::Vector3f _0x19c = {0, 0, 0};
    s32 mWaitShadowMaskLengthType = 0;
    s32 _0x1ac = 0;
    s32 mFlyAwayBySwitchTimer = 0;
    bool mIsLoop = false;
    bool _0x1b5 = false;
    bool _0x1b6 = false;
    const sead::Matrix34f* _0x1b8 = nullptr;
    sead::Vector3f _0x1c0 = {0, 0, 0};
    bool mIsGenerateRipple = false;
};

static_assert(sizeof(Bird) == 0x1d0);
