#pragma once

#include <math/seadVector.h>

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class AreaObj;
class CollisionPartsConnector;
class HitSensor;
class WaterSurfaceFinder;
}  // namespace al

class IJudge;
class IUsePlayerCollision;
class IUsePlayerHeightCheck;
class PlayerAnimator;
class PlayerCapManHeroEyesControl;
class PlayerConst;
class PlayerJointControlKeeper;
class PlayerModelChangerHakoniwa;
class PlayerTrigger;

struct PlayerJointParamCenterDynamics {
    s8 _0x0;
    bool _0x1;
    bool _0x2;
    bool _0x3;
    s32 _0x4;
    s64 _0x8;
    s32 _0x10;
    sead::Vector3f _0x14;
    sead::Vector3f _0x20;
};

struct PlayerJointParamGroundPose {
    bool _0x0;
};

class PlayerStateWait : public al::ActorStateBase {
public:
    PlayerStateWait(al::LiveActor* player, const PlayerConst* player_const,
                    const IUsePlayerCollision* collision,
                    const PlayerModelChangerHakoniwa* model_changer,
                    const PlayerJointControlKeeper* joint_control_keeper,
                    const al::WaterSurfaceFinder* water_surface_finder,
                    const IUsePlayerHeightCheck* height_check, const IJudge* judge,
                    PlayerAnimator* animator, PlayerTrigger* trigger,
                    PlayerCapManHeroEyesControl* cap_man_hero_eyes_control,
                    PlayerJointParamCenterDynamics* joint_param_center_dynamics,
                    PlayerJointParamGroundPose* joint_param_ground_pose);
    void appear() override;
    void kill() override;
    void control() override;
    bool isWait() const;
    bool isEnableLookAt() const;
    bool isEnableCancelAction() const;
    bool isEnableCancelHipDropJump() const;
    bool isEnableRecoveryLife() const;
    bool isEnableReactionCapCatch() const;
    bool isAreaAnimCapManHeroEyes() const;
    bool isPlaySwitchOnAreaAnim() const;
    bool isLandStain() const;
    bool isSleep() const;
    bool attackHipDropKnockDown(al::HitSensor* sender, al::HitSensor* receiver);
    bool tryClearIgnoreSwitchOnAreaAnim();
    bool requestAnimName(const char*);
    void noticeCarryStart();
    void initSceneStartAnim();
    bool tryConnectWait();
    f32 getInverseKinematicsRate() const;
    void exeLandStiffen();
    void exeLand();
    void exeWait();
    bool tryUpdateAreaAnim();
    bool tryChangeRequestAnim();
    bool tryChangeAreaAnim();
    bool tryGetSpecialStatusAnimName(const char**);
    void exeWaitConnect2D();
    void exeWaitRelaxStart();
    void exeWaitRelax();
    void exeWaitSnoozeStart();
    void exeWaitSnooze();
    void exeWaitSleepStart();
    void exeWaitSleep();
    void exeAreaAnimTurn();
    void exeAreaAnimWait();
    void exeRequestAnimWait();

private:
    const PlayerConst* mPlayerConst;
    const IUsePlayerCollision* mCollision;
    const PlayerModelChangerHakoniwa* mModelChanger;
    const PlayerJointControlKeeper* mJointControlKeeper;
    const al::WaterSurfaceFinder* mWaterSurfaceFinder;
    const IUsePlayerHeightCheck* mHeightCheck;
    const IJudge* mJudge;
    PlayerAnimator* mAnimator;
    PlayerTrigger* mTrigger;
    PlayerCapManHeroEyesControl* mCapManHeroEyesControl;
    PlayerJointParamCenterDynamics* mJointParamCenterDynamics;
    PlayerJointParamGroundPose* mJointParamGroundPose;
    bool _mIgnoreSwitchOnAreaAnim = false;
    al::AreaObj* _0x88 = nullptr;
    s64 _0x90 = 0;
    s64 _0x98 = 0;
    s32 _0xa0 = 0;
    s32 _0xa4 = 0;
    s32 _0xa8 = 0;
    s8 _0xac = 0;
    s8 _0xad = 0;
    s8 _0xae = 0;
    s32 _mUnknownTimer = 0;
    bool _0xb4 = false;
    const char* mAnimName = nullptr;
    sead::Vector3f _0xc0 = {0, 0, 0};
    s32 _0xcc = 0;
    al::CollisionPartsConnector* mCollisionPartsConnector = nullptr;
    sead::Vector3f _0xd8 = {0, 0, 0};
    bool _0xe4 = false;
};

static_assert(sizeof(PlayerStateWait) == 0xe8);

namespace rs {
void startCenterJointControl(PlayerJointParamCenterDynamics*, const al::LiveActor*,
                             const IUsePlayerCollision*, const sead::Vector3f&, bool);
}
