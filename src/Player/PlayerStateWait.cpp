#include "Player/PlayerStateWait.h"

#include "Library/Area/AreaObj.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchUtil.h"

#include "Npc/BirdPlayerGlideCtrl.h"
#include "Player/PlayerAnimator.h"
#include "Player/PlayerCapManHeroEyesControl.h"
#include "Player/PlayerConst.h"
#include "Player/PlayerFunction.h"
#include "Player/PlayerJointControlKeeper.h"
#include "Player/PlayerModelChangerHakoniwa.h"
#include "Player/PlayerTrigger.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "Util/JudgeUtil.h"
#include "Util/ObjUtil.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/PlayerUtil.h"

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

NERVES_MAKE_NOSTRUCT(PlayerStateWait, WaitRelax);
NERVES_MAKE_NOSTRUCT(PlayerStateWait, RequestAnimWait);
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
    : al::ActorStateBase("待機", player), mConst(player_const), mCollision(collision),
      mModelChanger(model_changer), mJointControlKeeper(joint_control_keeper),
      mWaterSurfaceFinder(water_surface_finder), mHeightCheck(height_check), mJudge(judge),
      mAnimator(animator), mTrigger(trigger), mCapManHeroEyesControl(cap_man_hero_eyes_control),
      mJointParamCenterDynamics(joint_param_center_dynamics),
      mJointParamGroundPose(joint_param_ground_pose) {
    mCollisionPartsConnector = al::createCollisionPartsConnector(player, sead::Quatf::unit);
    initNerve(&NrvPlayerStateWait.Wait, 0);
}

// TODO: might be a `sead` function?
static s32 roundToInt(f32 x) {
    return static_cast<s32>(x + (x >= 0 ? 0.5f : -0.5f));
}

static void getTemperature(s32* out, const al::LiveActor* player) {
    *out = 0;
    if (al::AreaObj* area = al::tryFindAreaObj(player, "TemperatureArea", al::getTrans(player)))
        al::tryGetAreaObjArg(out, area, "Temperature");
    if (*out == -4) {
        if (al::findIsInShade(player, al::getTrans(player)))
            *out = -2;
        else
            *out = 0;
    }
}

static void updateAreaAnim(const al::LiveActor* player,
                           const PlayerModelChangerHakoniwa* model_changer,
                           PlayerAnimator* animator, bool* is_ignore_switch, al::AreaObj** out_area,
                           const char** out_name, s32* out_start_frame,
                           sead::Vector3f* out_snap_front, bool* out_is_invalidate_input,
                           bool* out_is_use_area_valid, bool* out_is_sit_down,
                           bool* out_is_ignore_life_one) {
    al::AreaObj* area = al::tryFindAreaObj(player, "PlayerAnimArea", al::getTrans(player));
    if (*is_ignore_switch && *out_area == area && *out_area)
        return;
    *is_ignore_switch = false;
    *out_is_invalidate_input = false;
    *out_area = nullptr;
    *out_name = nullptr;
    *out_start_frame = 0;
    *out_is_use_area_valid = false;
    *out_is_sit_down = false;
    *out_is_ignore_life_one = false;
    *out_snap_front = {0, 0, 0};
    if (!area) {
        s32 temperature;
        getTemperature(&temperature, player);
        switch (model_changer->calcCostumeWarmLevel(temperature)) {
        case -3:
            *out_name = "WaitVeryCold";
            *out_start_frame = 0;
            break;
        case -2:
            *out_name = "WaitCold";
            *out_start_frame = 0;
            break;
        case -1:
        case 0:
        case 1:
            *out_name = nullptr;
            *out_start_frame = 0;
            break;
        case 2:
        case 3:
            *out_name = "WaitHot";
            *out_start_frame = roundToInt(animator->getMario3DWaitFrameMax());
            break;
        }
        return;
    }
    *out_area = area;
    al::tryGetAreaObjStringArg(out_name, area, "PlayerAnimName");
    al::tryGetAreaObjArg(out_is_use_area_valid, area, "IsUseAreaValid");
    al::tryGetAreaObjArg(out_start_frame, area, "AnimStartFrame");
    const char* name_snap_front = "None";
    al::tryGetAreaObjStringArg(&name_snap_front, area, "PlayerAnimNameSnapFront");
    if (!al::isEqualString(name_snap_front, "None")) {
        *out_name = name_snap_front;
        al::getAreaObjDirFront(out_snap_front, area);
    }
    if (*out_name)
        *out_is_sit_down = al::isEqualString(*out_name, "AreaWaitSitDown");
    al::tryGetAreaObjArg(out_is_invalidate_input, area, "IsInvalidateInput");
    al::tryGetAreaObjArg(out_is_ignore_life_one, area, "IsIgnoreLifeOne");
    if (al::isValidStageSwitch(area, "SwitchPlayerAnimEndOn"))
        *out_is_invalidate_input &= !al::isOnStageSwitch(area, "SwitchPlayerAnimEndOn");
}

static void calcLocalMovePowerGround(sead::Vector3f* out, const al::LiveActor* player,
                                     const IUsePlayerCollision* collision) {
    sead::Vector3f global_move_power = {0, 0, 0};
    if (rs::isCollidedGround(collision))
        rs::calcMovePowerGround(&global_move_power, collision, al::getTrans(player));
    sead::Quatf quat = sead::Quatf::unit;
    al::calcQuat(&quat, player);
    sead::Quatf inv_quat;
    inv_quat.setInverse(quat);
    out->setRotated(inv_quat, global_move_power);
    al::verticalizeVec(out, al::getGravity(player), *out);
}

void PlayerStateWait::appear() {
    _0xe4 = mTrigger->isOn(PlayerTrigger::EDemoEndTrigger_val0);
    al::disconnectMtxConnector(mCollisionPartsConnector);
    mIsAreaAnimInvalidateInput = false;
    updateAreaAnim(mActor, mModelChanger, mAnimator, &mIsIgnoreSwitchOnAreaAnim, &mAnimArea,
                   &mAreaAnimName, &mAreaAnimStartFrame, &mAreaAnimSnapFront,
                   &mIsAreaAnimInvalidateInput, &mIsAreaAnimUseAreaValid, &mIsAreaAnimSitDown,
                   &mIsAreaAnimIgnoreLifeOne);
    mAreaAnimStartTimer = mAreaAnimStartFrame;
    mRequestAnimName = nullptr;
    al::NerveStateBase::appear();
    mJointParamGroundPose->isLandJump3 = false;

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
            calcLocalMovePowerGround(&mLocalMovePowerGround, mActor, mCollision);
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
    al::setVelocity(mActor, ret - mConst->getGravity() * normal);

    if (is_judge) {
        al::setNerve(this, &NrvPlayerStateWait.LandStiffen);
        calcLocalMovePowerGround(&mLocalMovePowerGround, mActor, mCollision);
    } else {
        if (!mAnimator->isAnim("Jump3")) {
            rs::startCenterJointControl(mJointParamCenterDynamics, mActor, mCollision,
                                        al::getVelocity(mActor), true);
        }
        al::setNerve(this, &NrvPlayerStateWait.Land);
        calcLocalMovePowerGround(&mLocalMovePowerGround, mActor, mCollision);
    }
}

void PlayerStateWait::kill() {
    mJointParamCenterDynamics->_0x2 = true;
    mJointParamGroundPose->isLandJump3 = false;
    if (mAnimArea)
        al::tryOffStageSwitch(mAnimArea, "SwitchPlayerAnimKeepOn");
    rs::invalidateGlideBirdOnPlayerNose(mActor);
    al::endBgmSituation(mActor, "MarioSleep", false);
    al::NerveStateBase::kill();
}

void PlayerStateWait::control() {
    const al::LiveActor* player = mActor;
    sead::Vector3f global_move_power = {0, 0, 0};
    if (rs::isCollidedGround(mCollision))
        rs::calcMovePowerGround(&global_move_power, mCollision, al::getTrans(player));
    sead::Quatf quat = sead::Quatf::unit;
    al::calcQuat(&quat, player);
    sead::Quatf inv_quat;
    inv_quat.setInverse(quat);
    sead::Vector3f gravity = al::getGravity(player);
    sead::Vector3f old_move_power = mLocalMovePowerGround;
    sead::Vector3f new_move_power;
    new_move_power.setRotated(inv_quat, global_move_power);
    al::verticalizeVec(&new_move_power, gravity, new_move_power);
    sead::Vector3f diff = new_move_power - old_move_power;
    mLocalMovePowerGround.set(new_move_power);
    if (diff.length() < 3) {
        _0xcc = al::converge(_0xcc, 0, 1);
    } else {
        sead::Vector3f scaled_diff =
            -al::lerpValue(0.5f, 0.01f, al::easeOut(al::calcRate01(_0xcc, 0, 10))) * diff;
        mJointParamCenterDynamics->_0x1 = 1;
        mJointParamCenterDynamics->_0x20.setRotated(quat, scaled_diff);
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

static bool isInputInvalid(const PlayerStateWait* state, bool is_area_anim_invalidate_input) {
    if (al::isNerve(state, &NrvPlayerStateWait.AreaAnimTurn) ||
        al::isNerve(state, &NrvPlayerStateWait.AreaAnimWait))
        return is_area_anim_invalidate_input;
    return false;
}

bool PlayerStateWait::isEnableCancelAction() const {
    if (isInputInvalid(this, mIsAreaAnimInvalidateInput))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return mHipDropLandCancelFrame >= 0 &&
               al::isGreaterEqualStep(this, mHipDropLandCancelFrame);
    return true;
}

bool PlayerStateWait::isEnableCancelHipDropJump() const {
    if (isInputInvalid(this, mIsAreaAnimInvalidateInput))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen)) {
        return mHipDropLandCancelFrame >= 0 &&
               al::isInRangeStep(this, mConst->getJumpHipDropPermitBeginFrame(),
                                 mConst->getJumpHipDropPermitEndFrame());
    }
    return false;
}

bool PlayerStateWait::isEnableRecoveryLife() const {
    if (isDead())
        return false;
    if (isInputInvalid(this, mIsAreaAnimInvalidateInput))
        return false;
    if (al::isNerve(this, &NrvPlayerStateWait.LandStiffen))
        return mHipDropLandCancelFrame >= 0 &&
               al::isGreaterEqualStep(this, mHipDropLandCancelFrame);
    return true;
}

bool PlayerStateWait::isEnableReactionCapCatch() const {
    if (isDead())
        return true;
    return !mAnimArea || !al::isValidStageSwitch(mAnimArea, "SwitchPlayerAnimEndOn");
}

bool PlayerStateWait::isAreaAnimCapManHeroEyes() const {
    if (isDead())
        return false;
    if (!al::isNerve(this, &NrvPlayerStateWait.AreaAnimWait))
        return false;
    return mIsAreaAnimInvalidateInput && al::isGreaterEqualStep(this, 20);
}

bool PlayerStateWait::isPlaySwitchOnAreaAnim() const {
    if (isDead())
        return false;
    return isInputInvalid(this, mIsAreaAnimInvalidateInput);
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
    if (mHipDropLandCancelFrame < 0)
        return false;
    if (!al::isFirstStep(this))
        return false;
    return al::sendMsgPlayerHipDropKnockDown(receiver, sender);
}

bool PlayerStateWait::tryClearIgnoreSwitchOnAreaAnim() {
    if (!mIsIgnoreSwitchOnAreaAnim)
        return true;
    if (al::tryFindAreaObj(mActor, "PlayerAnimArea", al::getTrans(mActor)) == mAnimArea)
        return false;
    mIsIgnoreSwitchOnAreaAnim = false;
    return true;
}

bool PlayerStateWait::requestAnimName(const char* anim_name) {
    if (!mRequestAnimName) {
        mRequestAnimName = anim_name;
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
    s32 temperature;
    getTemperature(&temperature, mActor);
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

void PlayerStateWait::exeLandStiffen() {
    al::LiveActor* player = mActor;
    f32 extra_gravity = 0;
    if (al::isFirstStep(this)) {
        mHipDropLandCancelFrame = mConst->getHipDropLandCancelFrame();
        if (mAnimator->isAnim("SpinJumpDownFall"))
            mAnimator->startAnim("LandDownFall");
        else if (mAnimator->isAnim("SpinJumpDownFallL"))
            mAnimator->startAnim("LandDownFall");
        else if (mAnimator->isAnim("SpinJumpDownFallR"))
            mAnimator->startAnim("LandDownFall");
        else {
            al::startHitReaction(player, "高所着地");
            mAnimator->startAnim("LandStiffen");
            mHipDropLandCancelFrame = -1;
        }
        rs::landGroundPoseAndSnap(player, mTrigger, &extra_gravity, mCollision, mConst);
    }
    f32 gravity = mConst->getGravity() + extra_gravity;
    rs::waitGround(player, mCollision, gravity, mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnimEnd())
        al::setNerve(this, &NrvPlayerStateWait.Wait);
}

void PlayerStateWait::exeLand() {
    al::LiveActor* player = mActor;
    f32 extra_gravity = 0;
    if (al::isFirstStep(this)) {
        if (mAnimator->isAnim("JumpTurn"))
            mAnimator->startAnim("LandTurn");
        else if (mAnimator->isAnim("JumpBroad"))
            mAnimator->startAnim("LandRolling");
        else if (mAnimator->isAnim("Jump3"))
            mAnimator->startAnim("LandJump3");
        else
            mAnimator->startAnim("Land");
        mJointParamGroundPose->isLandJump3 = mAnimator->isAnim("LandJump3");
        rs::landGroundPoseAndSnap(player, mTrigger, &extra_gravity, mCollision, mConst);
    }
    f32 gravity = mConst->getGravity() + extra_gravity;
    rs::waitGround(player, mCollision, gravity, mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnimEnd()) {
        mJointParamGroundPose->isLandJump3 = false;
        al::setNerve(this, &NrvPlayerStateWait.Wait);
    }
}

void PlayerStateWait::exeWait() {
    al::LiveActor* player = mActor;
    tryUpdateAreaAnim();
    if (tryChangeRequestAnim())
        return;
    if (tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this)) {
        const char* anim_name = "Wait";
        tryGetSpecialStatusAnimName(&anim_name);
        if (!_0xe4)
            mAnimator->startAnim(anim_name);
        else if (!mAnimator->isAnim(anim_name))
            mAnimator->startAnim(anim_name);
    }
    sead::Vector3f normal = {0, 0, 0};
    rs::calcGroundNormalOrGravityDir(&normal, player, mCollision);
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnim("DamageWait") && !PlayerFunction::isPlayerHitPointOne(player)) {
        al::setNerve(this, &NrvPlayerStateWait.Wait);
        return;
    }
    if (al::isLessStep(this, 180))
        return;
    if (rs::isPlayerCarrySomething(player))
        return;
    if (mModelChanger->is2DModel())
        return;
    if (!mAnimator->isAnim("Wait"))
        return;
    al::setNerve(this, &NrvPlayerStateWait.WaitRelaxStart);
}

bool PlayerStateWait::tryUpdateAreaAnim() {
    if (mAnimArea)
        return false;
    const al::LiveActor* player = mActor;
    al::AreaObj* area = al::tryFindAreaObj(player, "PlayerAnimArea", al::getTrans(player));
    if (!area)
        return false;
    bool is_use_area_valid = false;
    al::tryGetAreaObjArg(&is_use_area_valid, area, "IsUseAreaValid");
    if (!is_use_area_valid)
        return false;
    updateAreaAnim(player, mModelChanger, mAnimator, &mIsIgnoreSwitchOnAreaAnim, &mAnimArea,
                   &mAreaAnimName, &mAreaAnimStartFrame, &mAreaAnimSnapFront,
                   &mIsAreaAnimInvalidateInput, &mIsAreaAnimUseAreaValid, &mIsAreaAnimSitDown,
                   &mIsAreaAnimIgnoreLifeOne);
    mAreaAnimStartTimer = mAreaAnimStartFrame;
    return true;
}

bool PlayerStateWait::tryChangeRequestAnim() {
    if (mModelChanger->is2DModel())
        return false;
    if (!mRequestAnimName)
        return false;
    if (rs::isPlayerCarrySomething(mActor))
        return false;
    rs::waitGround(mActor, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    mAnimator->startAnim(mRequestAnimName);
    al::setNerve(this, &RequestAnimWait);
    return true;
}

static bool turnToFrontForAreaAnim(al::LiveActor* player, const sead::Vector3f& front) {
    if (al::isNearZero(front))
        return true;
    sead::Vector3f up = {0, 0, 0};
    al::calcUpDir(&up, player);
    return al::turnToDirectionAxis(player, front, up, 10);
}

static void startAreaAnim(al::AreaObj* area, const char* name, PlayerAnimator* animator) {
    if (area)
        al::tryOnStageSwitch(area, "SwitchPlayerAnimKeepOn");
    animator->startAnim(name);
}

bool PlayerStateWait::tryChangeAreaAnim() {
    if (mIsIgnoreSwitchOnAreaAnim)
        return false;
    if (mModelChanger->is2DModel())
        return false;
    if (!mAreaAnimName)
        return false;
    if (rs::isPlayerCarrySomething(mActor))
        return false;
    const char* unused;
    if (tryGetSpecialStatusAnimName(&unused)) {
        if (!mAnimArea)
            return false;
        if (!al::isValidStageSwitch(mAnimArea, "SwitchPlayerAnimEndOn") && !mIsAreaAnimSitDown)
            return false;
    }
    if (mAnimArea && mIsAreaAnimUseAreaValid && !mAnimArea->isValid())
        return false;
    if (mAreaAnimStartTimer != 0) {
        mAreaAnimStartTimer = al::converge(mAreaAnimStartTimer, 0, 1);
        return false;
    }
    rs::waitGround(mActor, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (turnToFrontForAreaAnim(mActor, mAreaAnimSnapFront)) {
        startAreaAnim(mAnimArea, mAreaAnimName, mAnimator);
        al::setNerve(this, &NrvPlayerStateWait.AreaAnimWait);
    } else {
        al::setNerve(this, &NrvPlayerStateWait.AreaAnimTurn);
    }
    return true;
}

bool PlayerStateWait::tryGetSpecialStatusAnimName(const char** out) {
    if (mModelChanger->is2DModel())
        return false;
    const al::LiveActor* player = mActor;
    if (PlayerFunction::isPlayerHitPointOne(player) && !(mAnimArea && mIsAreaAnimIgnoreLifeOne)) {
        *out = "DamageWait";
        return true;
    }
    if (GameDataFunction::isRemovedCapByJango(player)) {
        *out = "BattleWait";
        return true;
    }
    if (rs::isSceneStatusBossBattleForPlayerAnim(player)) {
        *out = "BattleWait";
        return true;
    }
    return false;
}

void PlayerStateWait::exeWaitConnect2D() {
    al::LiveActor* player = mActor;
    if (al::isFirstStep(this))
        mAnimator->startAnim("Wait");
    sead::Vector3f normal = {0, 0, 0};
    rs::calcGroundNormalOrGravityDir(&normal, player, mCollision);
    rs::waitGround(player, mCollision, mConst->getGravityMove(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (!al::isMtxConnectorConnecting(mCollisionPartsConnector) &&
        rs::isCollidedGround(mCollision)) {
        sead::Vector3f up = {0, 0, 0};
        al::calcUpDir(&up, player);
        if (up.dot(rs::getCollidedGroundNormal(mCollision)) >=
            sead::Mathf::cos(sead::Mathf::deg2rad(1))) {
            sead::Vector3f move_power = {0, 0, 0};
            rs::calcMovePowerGround(&move_power, mCollision, rs::getCollidedGroundPos(mCollision));
            _0xd8 = rs::getCollidedGroundPos(mCollision) + move_power;
            al::attachMtxConnectorToCollisionParts(mCollisionPartsConnector,
                                                   rs::getCollidedGroundCollisionParts(mCollision));
        }
    }
    if (!mModelChanger->is2DModel())
        al::setNerve(this, &NrvPlayerStateWait.Wait);
}

void PlayerStateWait::exeWaitRelaxStart() {
    al::LiveActor* player = mActor;
    tryUpdateAreaAnim();
    if (tryChangeRequestAnim() || tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this))
        mAnimator->startAnim("WaitRelaxStart");
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnimEnd())
        al::setNerve(this, &WaitRelax);
}

void PlayerStateWait::exeWaitRelax() {
    al::LiveActor* player = mActor;
    tryUpdateAreaAnim();
    if (tryChangeRequestAnim() || tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this))
        mAnimator->startAnim("WaitRelax");
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (al::isGreaterEqualStep(this, 1200) &&
        !rs::isUnderWaterSurface(mWaterSurfaceFinder, mHeightCheck, 30))
        al::setNerve(this, &NrvPlayerStateWait.WaitSnoozeStart);
}

void PlayerStateWait::exeWaitSnoozeStart() {
    al::LiveActor* player = mActor;
    if (tryChangeRequestAnim() || tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this)) {
        mAnimator->startAnim("SnoozeStart");
        mCapManHeroEyesControl->requestWaitAnimChange("SnoozeStart");
    }
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnimEnd())
        al::setNerve(this, &NrvPlayerStateWait.WaitSnooze);
}

void PlayerStateWait::exeWaitSnooze() {
    al::LiveActor* player = mActor;
    if (tryChangeRequestAnim() || tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this)) {
        mAnimator->startAnim("Snooze");
        mCapManHeroEyesControl->requestWaitAnimChange("Snooze");
    }
    if (mAnimator->isAnim("Snooze") &&
        al::isNearZero(mAnimator->getAnimFrame() - mAnimator->getAnimFrameMax()))
        mCapManHeroEyesControl->requestWaitAnimChange("Snooze");
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (al::isGreaterEqualStep(this, 900))
        al::setNerve(this, &NrvPlayerStateWait.WaitSleepStart);
}

void PlayerStateWait::exeWaitSleepStart() {
    al::LiveActor* player = mActor;
    if (tryChangeRequestAnim() || tryChangeAreaAnim())
        return;
    if (al::isFirstStep(this)) {
        mAnimator->startAnim("SleepStart");
        mCapManHeroEyesControl->requestWaitAnimChange("SleepStart");
        al::startBgmSituation(mActor, "MarioSleep", false, true);
    }
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isAnimEnd())
        al::setNerve(this, &NrvPlayerStateWait.WaitSleep);
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
    if (mAnimator->isAnim("Sleep") &&
        al::isNearZero(mAnimator->getAnimFrame() - mAnimator->getAnimFrameMax()))
        mCapManHeroEyesControl->requestWaitAnimChange("Sleep");
    if (mAnimator->isSubAnimPlaying())
        rs::invalidateGlideBirdOnPlayerNose(player);
    else
        rs::validateGlideBirdOnPlayerNose(player);
    rs::waitGround(player, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
}

void PlayerStateWait::exeAreaAnimTurn() {
    if (al::isFirstStep(this))
        mAnimator->startAnim("TurnPoint");
    rs::waitGround(mActor, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (turnToFrontForAreaAnim(mActor, mAreaAnimSnapFront)) {
        startAreaAnim(mAnimArea, mAreaAnimName, mAnimator);
        al::setNerve(this, &NrvPlayerStateWait.AreaAnimWait);
    }
}

void PlayerStateWait::exeAreaAnimWait() {
    rs::waitGround(mActor, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mIsAreaAnimSitDown && al::isStep(mActor, 480))
        GameDataFunction::recoveryPlayerMax(mActor);
    if (mAnimArea && mIsAreaAnimUseAreaValid && !mAnimArea->isValid()) {
        al::setNerve(this, &NrvPlayerStateWait.Wait);
        return;
    }
    if (mAnimator->isCurrentAnimOneTime() && mAnimator->isAnimEnd()) {
        if (mAnimArea) {
            al::tryOnStageSwitch(mAnimArea, "SwitchPlayerAnimEndOn");
            al::tryOffStageSwitch(mAnimArea, "SwitchPlayerAnimKeepOn");
        }
        mIsIgnoreSwitchOnAreaAnim = true;
        mIsAreaAnimInvalidateInput = false;
        mAreaAnimStartTimer = mAreaAnimStartFrame;
        al::setNerve(this, &NrvPlayerStateWait.Wait);
    }
}

void PlayerStateWait::exeRequestAnimWait() {
    rs::waitGround(mActor, mCollision, mConst->getGravity(), mConst->getFallSpeedMax(),
                   mConst->getSlerpQuatRateWait(), mConst->getWaitPoseDegreeMax());
    if (mAnimator->isCurrentAnimOneTime() && mAnimator->isAnimEnd())
        al::setNerve(this, &NrvPlayerStateWait.Wait);
}
