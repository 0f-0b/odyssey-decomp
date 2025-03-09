#include "Player/PlayerStateWait.h"

#include <math/seadQuat.h>

#include "Library/Area/AreaObj.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchKeeper.h"
#include "Library/Stage/StageSwitchUtil.h"

#include "Npc/BirdPlayerGlideCtrl.h"
#include "Player/PlayerAnimator.h"
#include "Player/PlayerCapManHeroEyesControl.h"
#include "Player/PlayerConst.h"
#include "Player/PlayerJointControlKeeper.h"
#include "Player/PlayerModelChangerHakoniwa.h"
#include "Player/PlayerTrigger.h"
#include "Util/JudgeUtil.h"
#include "Util/ObjUtil.h"
#include "Util/PlayerCollisionUtil.h"

namespace {
NERVE_IMPL(PlayerStateWait, Wait);
NERVE_IMPL(PlayerStateWait, LandStiffen);
NERVE_IMPL(PlayerStateWait, Land);
NERVE_IMPL(PlayerStateWait, WaitConnect2D);
NERVE_IMPL(PlayerStateWait, WaitSnoozeStart);
NERVE_IMPL(PlayerStateWait, WaitSnooze);
NERVE_IMPL(PlayerStateWait, WaitSleepStart);
NERVE_IMPL(PlayerStateWait, WaitSleep);
NERVE_IMPL(PlayerStateWait, AreaAnimWait);
NERVE_IMPL(PlayerStateWait, WaitRelaxStart);
NERVE_IMPL(PlayerStateWait, WaitRelax);
NERVE_IMPL(PlayerStateWait, AreaAnimTurn);
NERVE_IMPL(PlayerStateWait, RequestAnimWait);

[[gnu::used]] NERVES_MAKE_NOSTRUCT(PlayerStateWait, WaitRelax);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(PlayerStateWait, RequestAnimWait);
NERVES_MAKE_STRUCT(PlayerStateWait, Wait, LandStiffen, Land, WaitConnect2D, WaitSnoozeStart,
                   WaitSnooze, WaitSleepStart, WaitSleep, AreaAnimWait, WaitRelaxStart,
                   AreaAnimTurn);
}  // namespace

PlayerStateWait::PlayerStateWait(al::LiveActor* player, const PlayerConst* player_const,
                                 const IUsePlayerCollision* collision,
                                 const PlayerModelChangerHakoniwa* model_changer,
                                 const PlayerJointControlKeeper* joint_control_keeper,
                                 const al::WaterSurfaceFinder* water_surface_finder,
                                 const IUsePlayerHeightCheck* height_check, const IJudge* judge,
                                 PlayerAnimator* animator, PlayerTrigger* trigger,
                                 PlayerCapManHeroEyesControl* cap_man_hero_eyes_control,
                                 PlayerJointParamCenterDynamics* joint_param_center_dynamics,
                                 PlayerJointParamGroundPose* joint_param_ground_pose)
    : al::ActorStateBase("待機", player), mPlayerConst(player_const), mCollision(collision),
      mModelChanger(model_changer), mJointControlKeeper(joint_control_keeper),
      mWaterSurfaceFinder(water_surface_finder), mHeightCheck(height_check), mJudge(judge),
      mAnimator(animator), mTrigger(trigger), mCapManHeroEyesControl(cap_man_hero_eyes_control),
      mJointParamCenterDynamics(joint_param_center_dynamics),
      mJointParamGroundPose(joint_param_ground_pose) {
    mCollisionPartsConnector = al::createCollisionPartsConnector(player, sead::Quatf::unit);
    initNerve(&NrvPlayerStateWait.Wait, 0);
}

static void f_0x48cad0(al::LiveActor*, const PlayerModelChangerHakoniwa* model_changer,
                       PlayerAnimator* animator, bool*, al::AreaObj**, s64*, s32*, s64*, bool*, s8*,
                       s8*, s8*);
static void f_0x48cdb0(sead::Vector3f*, al::LiveActor*, const IUsePlayerCollision*);

void PlayerStateWait::appear() {
    _0xe4 = mTrigger->isOn(PlayerTrigger::EDemoEndTrigger_val0);
    al::disconnectMtxConnector(mCollisionPartsConnector);
    _0xb4 = false;
    f_0x48cad0(mActor, mModelChanger, mAnimator, &_mIgnoreSwitchOnAreaAnim, &_0x88, &_0x90, &_0xa4,
               &_0x98, &_0xb4, &_0xac, &_0xad, &_0xae);
    _0xa8 = _0xa4;
    mAnimName = nullptr;
    setDead(false);
    mJointParamGroundPose->_0x0 = false;

    bool is_judge = rs::isJudge(mJudge);
    if (is_judge) {
        _0xcc = 0;
    } else {
        if (!(mTrigger->isOn(PlayerTrigger::ECollisionTrigger_val8) ||
              mTrigger->isOn(PlayerTrigger::EActionTrigger_val24))) {
            rs::startHitReactionLandIfLanding(mActor, mCollision, mModelChanger->is2DModel());
        }

        _0xcc = 0;

        const IUsePlayerCollision* collision = mCollision;
        const PlayerModelChangerHakoniwa* model_changer = mModelChanger;
        const PlayerTrigger* trigger = mTrigger;
        if (!rs::isJustLand(collision) || model_changer->is2DModel() ||
            trigger->isOn(PlayerTrigger::EActionTrigger_val24)) {
            mJointParamCenterDynamics->_0x3 = mTrigger->isOn(PlayerTrigger::EActionTrigger_val25);

            if (mJointParamCenterDynamics->_0x3) {
                rs::startCenterJointControl(mJointParamCenterDynamics, mActor, mCollision,
                                            al::getVelocity(mActor), false);
                sead::Vector3f normal = {0, 0, 0};
                rs::calcGroundNormalOrGravityDir(&normal, mActor, mCollision);
                sead::Vector3f ret = {0, 0, 0};
                mJointControlKeeper->calcGroundPoseUp(&ret);
                al::alongVectorNormalH(&mJointParamCenterDynamics->_0x14,
                                       mJointParamCenterDynamics->_0x14, normal, ret);
            }

            f_0x48cdb0(&_0xc0, mActor, mCollision);

            if (al::isInAreaObj(mActor, "PlayerMoveSmallPlanetArea"))
                al::setNerve(this, &NrvPlayerStateWait.WaitConnect2D);
            else
                al::setNerve(this, &NrvPlayerStateWait.Wait);
            return;
        }
    }

    sead::Vector3f normal = {0, 0, 0};
    rs::calcGroundNormalOrGravityDir(&normal, mActor, mCollision);
    sead::Vector3f ret = {0, 0, 0};
    al::alongVectorNormalH(&ret, al::getVelocity(mActor), -al::getGravity(mActor), normal);
    al::setVelocity(mActor, ret - mPlayerConst->getGravity() * normal);

    if (is_judge) {
        al::setNerve(this, &NrvPlayerStateWait.LandStiffen);
        f_0x48cdb0(&_0xc0, mActor, mCollision);
    } else {
        if (!mAnimator->isAnim("Jump3")) {
            rs::startCenterJointControl(mJointParamCenterDynamics, mActor, mCollision,
                                        al::getVelocity(mActor), true);
        }
        al::setNerve(this, &NrvPlayerStateWait.Land);
        f_0x48cdb0(&_0xc0, mActor, mCollision);
    }
}

void PlayerStateWait::kill() {
    mJointParamCenterDynamics->_0x2 = true;
    mJointParamGroundPose->_0x0 = false;
    if (_0x88)
        al::tryOffStageSwitch(_0x88, "SwitchPlayerAnimKeepOn");
    rs::invalidateGlideBirdOnPlayerNose(mActor);
    al::endBgmSituation(mActor, "MarioSleep", false);
    setDead(true);
}

void PlayerStateWait::control() {
    const al::LiveActor* player = mActor;

    // sp+0x30
    sead::Vector3f move_power = {0, 0, 0};
    if (rs::isCollidedGround(mCollision))
        rs::calcMovePowerGround(&move_power, mCollision, al::getTrans(player));

    // sp+0x20
    sead::Quatf quat = sead::Quatf::unit;
    al::calcQuat(&quat, player);

    f32 w = quat.w;
    f32 x = quat.x;
    f32 y = quat.y;
    f32 z = quat.z;
    if (f32 len_sq = quat.dot(quat); len_sq > sead::MathCalcCommon<f32>::epsilon()) {
        f32 inv_len_sq = 1.0f / len_sq;
        w *= inv_len_sq;
        x *= inv_len_sq;
        y *= inv_len_sq;
        z *= inv_len_sq;
    }

    // sp+0x10
    sead::Vector3f gravity = al::getGravity(player);

    // sp
    sead::Vector3f rotated = move_power;
    rotated.rotate(sead::Quatf{x, y, z, w});
    al::verticalizeVec(&rotated, gravity, rotated);

    sead::Vector3f s8_s9_s10 = rotated - _0xc0;
    _0xc0 = rotated;
    if (s8_s9_s10.length() < 3) {
        _0xcc = al::converge(_0xcc, 0, 1);
    } else {
        f32 multiplier = -al::lerpValue(
            0.5f, 0.01f, al::easeOut(al::calcRate01(static_cast<f32>(_0xcc), 0, 10)));
        sead::Vector3f s1_s2_s0 = s8_s9_s10 * multiplier;
        mJointParamCenterDynamics->_0x1 = 1;
        s1_s2_s0.rotate(quat);
        mJointParamCenterDynamics->_0x20 = s1_s2_s0;
        al::limitLength(&mJointParamCenterDynamics->_0x20, mJointParamCenterDynamics->_0x20, 4);
        _0xcc = 10;
    }
}

bool PlayerStateWait::isWait() const {
    if (isDead())
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.Land))
        return false;
    return true;
}

bool PlayerStateWait::isEnableLookAt() const {
    if (isDead())
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.Land))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.WaitSnoozeStart))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.WaitSnooze))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.WaitSleepStart))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.WaitSleep))
        return false;
    return true;
}

static bool isAreaAnim(const PlayerStateWait* state, bool _param) {
    if (al::isNerve(state, &NrvPlayerStateWait.AreaAnimTurn) ||
        al::isNerve(state, &NrvPlayerStateWait.AreaAnimWait))
        return _param;
    return false;
}

bool PlayerStateWait::isEnableCancelAction() const {
    if (isAreaAnim(this, _0xb4))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return _mUnknownTimer >= 0 && al::isGreaterEqualStep(this, _mUnknownTimer);
    return true;
}

bool PlayerStateWait::isEnableCancelHipDropJump() const {
    if (isAreaAnim(this, _0xb4))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen)) {
        return _mUnknownTimer >= 0 &&
               al::isInRangeStep(this, mPlayerConst->getJumpHipDropPermitBeginFrame(),
                                 mPlayerConst->getJumpHipDropPermitEndFrame());
    }
    return false;
}

bool PlayerStateWait::isEnableRecoveryLife() const {
    if (isDead())
        return false;
    if (isAreaAnim(this, _0xb4))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return _mUnknownTimer >= 0 && al::isGreaterEqualStep(this, _mUnknownTimer);
    return true;
}

bool PlayerStateWait::isEnableReactionCapCatch() const {
    if (isDead())
        return true;
    return !_0x88 || !al::isValidStageSwitch(_0x88, "SwitchPlayerAnimEndOn");
}

bool PlayerStateWait::isAreaAnimCapManHeroEyes() const {
    if (isDead())
        return false;
    if (!al::isNerve(this, &NrvPlayerStateWait.AreaAnimWait))
        return false;
    return _0xb4 && al::isGreaterEqualStep(this, 20);
}

bool PlayerStateWait::isPlaySwitchOnAreaAnim() const {
    if (isDead())
        return false;
    return isAreaAnim(this, _0xb4);
}

bool PlayerStateWait::isLandStain() const {
    if (isDead())
        return false;
    if (!al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return false;
    return rs::isCollidedGround(mCollision);
}

bool PlayerStateWait::isSleep() const {
    if (isDead())
        return false;
    if (!al::isNerve(this, &NrvPlayerStateWait.WaitSleep))
        return false;
    return !mAnimator->isSubAnimPlaying();
}

bool PlayerStateWait::attackHipDropKnockDown(al::HitSensor* sender, al::HitSensor* receiver) {
    if (isDead())
        return false;
    if (!al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return false;
    if (_mUnknownTimer < 0)
        return false;
    if (!al::isFirstStep(this))
        return false;
    return al::sendMsgPlayerHipDropKnockDown(receiver, sender);
}

bool PlayerStateWait::tryClearIgnoreSwitchOnAreaAnim() {
    if (!_mIgnoreSwitchOnAreaAnim)
        return true;
    if (al::tryFindAreaObj(mActor, "PlayerAnimArea", al::getTrans(mActor)) == _0x88)
        return false;
    _mIgnoreSwitchOnAreaAnim = false;
    return true;
}

bool PlayerStateWait::requestAnimName(const char* name) {
    if (!mAnimName) {
        mAnimName = name;
        return true;
    }
    return false;
}

void PlayerStateWait::noticeCarryStart() {
    if (isDead())
        return;
    if (mAnimator->isAnim("Wait"))
        return;
    mAnimator->startAnim("Wait");
    al::setNerve(this, &NrvPlayerStateWait.Wait);
}

void PlayerStateWait::initSceneStartAnim() {
    const al::LiveActor* player = mActor;

    s32 temperature = 0;
    if (al::AreaObj* area = al::tryFindAreaObj(player, "TemperatureArea", al::getTrans(player)))
        al::tryGetAreaObjArg(&temperature, area, "Temperature");
    if (temperature == -4) {
        if (al::findIsInShade(player, al::getTrans(player)))
            temperature = -2;
        else
            temperature = 0;
    }

    switch (mModelChanger->calcCostumeWarmLevel(temperature)) {
    case -3:
        mAnimator->startAnim("WaitVeryCold");
        break;
    case -2:
        mAnimator->startAnim("WaitCold");
        break;
    }

    mAnimator->clearInterpolation();
}

bool PlayerStateWait::tryConnectWait() {
    if (al::isNerve(this, &NrvPlayerStateWait.WaitConnect2D) &&
        al::isMtxConnectorConnecting(mCollisionPartsConnector)) {
        al::calcConnectTrans(al::getTransPtr(mActor), mCollisionPartsConnector, _0xd8);
        return true;
    }
    return false;
}

f32 PlayerStateWait::getInverseKinematicsRate() const {
    if (isDead())
        return 0;
    if (al::isNerve(this, &NrvPlayerStateWait.WaitSleep))
        return 0;
    if (!al::isNerve(this, &NrvPlayerStateWait.WaitSleepStart))
        return 1;
    return sead::Mathf::clamp(1 - al::calcNerveRate(this, 120), 0, 1);
}

void PlayerStateWait::exeWaitSleep() {
    al::LiveActor* player = mActor;

    if (tryChangeRequestAnim() || tryChangeAreaAnim()) {
        rs::invalidateGlideBirdOnPlayerNose(player);
        return;
    }

    if (al::isFirstStep(this)) {
        mAnimator->startAnim("Sleep");
        mCapManHeroEyesControl->requestWaitAnimChange("Sleep");
    }

    if (mAnimator->isAnim("Sleep")) {
        f32 frame = mAnimator->getAnimFrame();
        f32 max = mAnimator->getAnimFrameMax();
        if (al::isNearZero(frame - max, 0.001f))
            mCapManHeroEyesControl->requestWaitAnimChange("Sleep");
    }

    if (mAnimator->isSubAnimPlaying())
        rs::invalidateGlideBirdOnPlayerNose(player);
    else
        rs::validateGlideBirdOnPlayerNose(player);

    rs::waitGround(player, mCollision, mPlayerConst->getGravity(), mPlayerConst->getFallSpeedMax(),
                   mPlayerConst->getSlerpQuatRateWait(), mPlayerConst->getWaitPoseDegreeMax());
}
