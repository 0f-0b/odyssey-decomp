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
    bool isLandJump3;
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
    bool requestAnimName(const char* anim_name);
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
    bool tryGetSpecialStatusAnimName(const char** out);
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
    const PlayerConst* mConst;
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
    bool mIsIgnoreSwitchOnAreaAnim = false;
    al::AreaObj* mAnimArea = nullptr;
    const char* mAreaAnimName = nullptr;
    sead::Vector3f mAreaAnimSnapFront = {0, 0, 0};
    s32 mAreaAnimStartFrame = 0;
    s32 mAreaAnimStartTimer = 0;
    bool mIsAreaAnimUseAreaValid = false;
    bool mIsAreaAnimSitDown = false;
    bool mIsAreaAnimIgnoreLifeOne = false;
    s32 mHipDropLandCancelFrame = 0;
    bool mIsAreaAnimInvalidateInput = false;
    const char* mRequestAnimName = nullptr;
    sead::Vector3f mLocalMovePowerGround = {0, 0, 0};
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
