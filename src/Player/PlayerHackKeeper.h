#pragma once

#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class LiveActor;
class HitSensor;
class CollisionPartsFilterBase;
struct ActorInitInfo;
}  // namespace al

class PlayerRecoverySafetyPoint;
class HackCap;
class PlayerInput;
class PlayerDamageKeeper;
class IPlayerModelChanger;
class IUsePlayerHeightCheck;
struct HackObjInfo;
class PlayerCollider;
class CapTargetInfo;
class PlayerHackStartTexKeeper;
class IUsePlayerCollision;
class IUsePlayerHack;

struct HackEndParam {
    sead::Vector3f vel = {0.0f, 0.0f, 0.0f};
    sead::Vector3f dir = {0.0f, 0.0f, 0.0f};
    sead::Quatf quat = sead::Quatf::unit;
    sead::Vector3f targetPos = {0.0f, 0.0f, 0.0f};
    f32 escapeScale = 1.0f;
    s32 delayFrames = 0;
    bool isDamage = false;
    bool hasVelocity = false;
};

class PlayerHackKeeper {
public:
    PlayerHackKeeper(al::LiveActor* player, HackCap* cap, PlayerRecoverySafetyPoint* safety_point,
                     const PlayerInput* input, const sead::Matrix34f* mtx,
                     const PlayerDamageKeeper* damageKeeper,
                     const IPlayerModelChanger* modelChanger,
                     const IUsePlayerHeightCheck* heightCheck);

    void createHackModel(const al::ActorInitInfo& info);
    void startHack(al::HitSensor*, al::HitSensor*, al::LiveActor*);
    void setupHack(al::HitSensor*, al::HitSensor*, al::LiveActor*);
    void endHack(const HackEndParam*);
    void endHackStartDemo(al::LiveActor*);
    void startHackStartDemo(al::LiveActor*);
    void startHackStartDemoPuppetable(al::LiveActor*);
    void addHackStartDemo(al::LiveActor*);
    void appearHackDemoModel(const sead::Matrix34f&, f32);
    void updateHackDemoModel(const sead::Matrix34f&, f32);
    void deleteHackDemoModelEffect();
    void killHackDemoModel();
    bool isActiveHackStartDemo() const;
    void recordHack();
    void cancelHackArea();
    void cancelHack();
    void cancelForceRecovery();
    void tryEscapeHack();
    void sendTransferHack();
    void sendMarioCheckpointFlagWarp();
    void sendMarioDemo();
    void forceKillHack();
    void sendMarioDead();
    void sendMarioInWater();
    void sendMarioDeathArea();
    void sendMsgEnableMapCheckPointWarp();
    void sendMsgSelfCeilingCheckMiss();
    bool receiveRequestTransferHack(al::HitSensor*);
    bool requestDamage();
    void receiveRequestDamage();
    void sendSyncDamageVisibility();
    void pushWorldEndBorder(const sead::Vector3f&);
    const char* getCurrentHackName() const;
    const IUsePlayerCollision* getPlayerCollision() const;
    f32 getHackGuideHeight() const;
    bool isHackGuideEnable() const;
    f32 getHackStayGravityMargine() const;
    al::CollisionPartsFilterBase* getCollisionPartsFilter() const;
    bool isHackGroupTalkScare() const;
    bool isHackNoCollisionMsg() const;
    bool isHackNoSeparateCameraInput() const;
    bool isHackUsePlayerCollision() const;
    bool isHackCancelCeilingCheck() const;
    bool isHackInvalidLifeRecovery() const;
    void requestForceHackStageStart(al::HitSensor*, const CapTargetInfo*, al::LiveActor*);
    bool executeForceHackStageStart(al::HitSensor*, IUsePlayerHack*);
    void startDemo();
    void endDemo();

    al::LiveActor* getParent() const { return mParent; }

    HackCap* getHackCap() const { return mHackCap; }

    PlayerRecoverySafetyPoint* getRecoverySafePoint() const { return mRecoverySafePoint; }

    const PlayerInput* getInput() const { return mInput; }

    const PlayerDamageKeeper* getDamageKeeper() const { return mDamageKeeper; }

    const IPlayerModelChanger* getModelChanger() const { return mModelChanger; }

    const IUsePlayerHeightCheck* getHeightCheck() const { return mHeightCheck; }

    void setPuppetable(bool isPuppetable) { mIsPuppetable = isPuppetable; }

    bool isPuppetable() const { return mIsPuppetable; }

    bool isCancellingHack() const { return mIsCancellingHack; }

    bool isPuppetable2() const { return mIsPuppetable2; }

    bool isHack() const { return mIsHack; }

    bool isHackDemoStarted() const { return mIsHackDemoStarted; }

    bool isStartedHacking() const { return mIsStartedHacking; }

    al::LiveActor* getHack() const { return mHackActor; }

    al::HitSensor* getHackSensor() const { return mHackHitSensor; }

private:
    al::LiveActor* mParent;
    HackCap* mHackCap;
    PlayerRecoverySafetyPoint* mRecoverySafePoint;
    s64 field_18 = 0;
    s64 field_20 = 0;
    const PlayerInput* mInput;
    const sead::Matrix34f* _30;
    const PlayerDamageKeeper* mDamageKeeper;
    const IPlayerModelChanger* mModelChanger;
    const IUsePlayerHeightCheck* mHeightCheck;
    al::HitSensor* mParentBodySensor = nullptr;
    bool mIsPuppetable = false;
    bool mIsCancellingHack = false;
    bool mIsHackDemoStarted = false;
    bool mIsPuppetable2 = false;
    bool mIsStartedHacking = false;
    bool mIsHack = false;
    bool mIsTookDamage = false;
    al::CollisionPartsFilterBase* mCollisionFilter;
    al::LiveActor* mHackActor = nullptr;
    al::HitSensor* mHackHitSensor = nullptr;
    HackObjInfo* mHackObjectInfo = nullptr;
    al::HitSensor* mStageStartActorSensor = nullptr;
    al::LiveActor* mStageStartActor = nullptr;
    CapTargetInfo* mStageStartCapTargetInfo = nullptr;
    PlayerHackStartTexKeeper* mHackStartTexKeeper = nullptr;
    al::LiveActor* mHackModel = nullptr;
    sead::PtrArray<sead::Matrix34f> mHackModelSlices;
    sead::Matrix34f* field_b8 = nullptr;
    s32 field_c0 = 0;
    s32 field_c4 = 0;
    s32 field_c8 = 0;
};

static_assert(sizeof(PlayerHackKeeper) == 0xd0);
