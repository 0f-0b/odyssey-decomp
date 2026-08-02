#include "Npc/Bird.h"

#include "Library/Collision/Collider.h"
#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/CollisionPartsTriangle.h"
#include "Library/Collision/PartsConnector.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Demo/DemoFunction.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Execute/ExecuteUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/RailUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"
#include "Library/Stage/StageSwitchUtil.h"

#include "Npc/BigBirdStateFlyAway.h"
#include "Npc/BigBirdStateWait.h"
#include "Npc/SmallBirdStateFlyAway.h"
#include "Npc/SmallBirdStateWait.h"
#include "Util/DemoUtil.h"
#include "Util/ItemGenerator.h"
#include "Util/NpcAnimUtil.h"
#include "Util/PlayerCollisionUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"
#include "Util/WaterSurfaceShadow.h"

namespace {
NERVE_END_IMPL(Bird, Wait);
NERVE_IMPL(Bird, Freeze);
NERVE_END_IMPL(Bird, FlyAway);
NERVE_IMPL(Bird, FlyRail);
NERVE_IMPL(Bird, Delay);
NERVE_END_IMPL(Bird, GatheringFallDown);
NERVE_IMPL(Bird, GatheringFallDownSlow);
NERVE_IMPL(Bird, GatheringLand);
NERVE_IMPL(Bird, FlyRailGoal);
NERVE_IMPL(Bird, Reset);

NERVES_MAKE_NOSTRUCT(Bird, Delay);
NERVES_MAKE_NOSTRUCT(Bird, Reset);
NERVES_MAKE_STRUCT(Bird, Wait, Freeze, FlyAway, FlyRail, GatheringFallDown, GatheringFallDownSlow,
                   GatheringLand, FlyRailGoal);
}  // namespace

Bird::Bird(const char* name) : al::LiveActor(name), mInitialNerve(&NrvBird.Wait) {
    _0x108 = new Struct_0x108;
}

void Bird::init(const al::ActorInitInfo& info) {
    if (mArchiveName)
        al::initActorWithArchiveName(this, info, mArchiveName, nullptr);
    else
        al::initActor(this, info);
    al::initNerve(this, &NrvBird.Wait, 2);
    mHostAnimInfo = new HostAnimInfo;
    if (al::isExistDepthShadowMapCtrl(this))
        al::invalidateDepthShadowMap(this);

    bool is_freeze = false;
    if (al::tryGetArg(&is_freeze, info, "IsFreeze") && is_freeze) {
        al::invalidateHitSensors(this);
        al::offCollide(this);
        al::setNerve(this, &NrvBird.Freeze);
        makeActorAlive();
        return;
    }

    al::tryGetArg(&mIsLoop, info, "IsLoop");
    al::tryGetArg(&mWaitShadowMaskLengthType, info, "WaitShadowMaskLengthType");
    al::tryGetArg(&mIsGenerateRipple, info, "IsGenerateRipple");
    if (_0x1b5) {
        al::invalidateShadowMask(this);
        al::registerExecutorActorDraw(this, info.executeDirector, "デプスシャドウ[キャラクター]");
    }
    al::syncSensorScaleY(this);

    Struct_0x108* _x22 = _0x108;
    al::tryGetArg(&_x22->playerWatchDistance, info, "PlayerWatchDistance");
    al::tryGetArg(&_x22->waitStepAtRailEdge, info, "WaitStepAtRailEdge");
    al::tryGetArg(&_x22->isInvalidTurnReverse, info, "IsInvalidTurnReverse");
    al::tryGetArg(&_x22->flyAwayBySwitchDelayStep, info, "FlyAwayBySwitchDelayStep");

    if (al::isEqualString(al::getModelName(this), "BirdEagle") ||
        al::isEqualString(al::getModelName(this), "BirdGull")) {
        mStateWait = new BigBirdStateWait(this);
        al::initNerveState(this, mStateWait, &NrvBird.Wait, "待機");
        mStateFlyAway = new BigBirdStateFlyAway(this);
    } else {
        mStateWait = new SmallBirdStateWait(this, info);
        al::initNerveState(this, mStateWait, &NrvBird.Wait, "待機");
        mStateFlyAway = new SmallBirdStateFlyAway(this);
    }
    mStateFlyAway->initByPlacementInfo(info);
    al::initNerveState(this, mStateFlyAway, &NrvBird.FlyAway, "飛び去り");
    mFlyAwayBySwitchTimer = _0x108->flyAwayBySwitchDelayStep;

    if (al::isPlaced(info)) {
        sead::Quatf quat = sead::Quatf::unit;
        al::calcQuat(&quat, this);
        mMtxConnector = al::tryCreateCollisionPartsConnector(this, info, quat);

        if (al::isExistRail(this)) {
            if (al::isExistLinkChild(info, "StartPos", 0)) {
                sead::Quatf start_quat = sead::Quatf::unit;
                al::getLinksQT(&start_quat, al::getTransPtr(this), al::getPlacementInfo(info),
                               "StartPos");
                al::calcQuatFront(al::getFrontPtr(this), start_quat);
                al::normalize(al::getFrontPtr(this));
            } else {
                al::setSyncRailToStart(this);
                al::setFront(this, al::getRailDir(this));
                if (al::tryGetBoolArgOrFalse(info, "IsMoveStartImmediately")) {
                    al::getTransPtr(this)->set(al::getRailPos(this) + sead::Vector3f{0, 200, 0});
                    al::setNerve(this, &NrvBird.FlyRail);
                }
            }
            al::setRailClippingInfo(new sead::Vector3f{0, 0, 0}, this, 100, 150);
            if (al::isExistDepthShadowMapCtrl(this)) {
                al::invalidateShadowMask(this);
                al::validateDepthShadowMap(this);
            }
        }

        mItemGenerator = new ItemGenerator;
        mItemGenerator->initNoLinkShine(this, info, false);
        if (mItemGenerator->isShine())
            al::startHitReaction(this, "シャイン保持");

        bool use_water_surface_shadow = false;
        if (al::tryGetArg(&use_water_surface_shadow, info, "UseWaterSurfaceShadow") &&
            use_water_surface_shadow) {
            mWaterSurfaceShadow = new WaterSurfaceShadow(info, "WaterSurfaceShadow");
            mWaterSurfaceShadow->setScale(0.65f);
            mWaterSurfaceShadow->setMaxDistance(2500, 0.25);
        }
    }

    mInitialNerve = getNerveKeeper()->getCurrentNerve();
    mInitialTrans.set(al::getTrans(this));
    mInitialFront.set(al::getFront(this));
    al::registActorToDemoInfo(this, info);
    al::trySyncStageSwitchAppear(this);
}

static void resetHostAnimInfo(Bird::HostAnimInfo* info, const al::CollisionParts* collision_parts) {
    info->sklAnimName = nullptr;
    info->sklAnimTime = -1;
    info->collisionParts = collision_parts;
    info->initialTrans.set(al::getTrans(collision_parts->getConnectedHost()));
}

static void rotateAndTranslate(sead::Vector3f* out, const sead::Matrix34f& m,
                               const sead::Vector3f& v) {
    m.getTranslation(*out);
    sead::Quatf quat = sead::Quatf::unit;
    m.toQuat(quat);
    if (!al::isNearZero(v)) {
        sead::Vector3f rotated = v;
        rotated.rotate(quat);
        *out += rotated;
    }
}

void Bird::initAfterPlacement() {
    if (mMtxConnector || (!_0x1b8 && al::isNerve(this, &NrvBird.Wait))) {
        if (const al::CollisionParts* collision_parts =
                alCollisionUtil::getStrikeArrowCollisionParts(
                    this, nullptr, al::getTrans(this) + sead::Vector3f{0, 50, 0}, {0, -150, 0},
                    nullptr, nullptr)) {
            if (mMtxConnector) {
                al::attachMtxConnectorToCollisionParts(mMtxConnector, collision_parts);
                al::calcConnectQT(&_0x140, al::getTransPtr(this), mMtxConnector);
            }
            resetHostAnimInfo(mHostAnimInfo, collision_parts);
        }
    }
    if (al::isAlive(this) && _0x1b8) {
        rotateAndTranslate(al::getTransPtr(this), *_0x1b8, _0x1c0);
        al::resetPosition(this);
    }
}

void Bird::initGlideOn(const sead::Matrix34f* _arg1, const sead::Vector3f& _arg2) {
    _0x1b8 = _arg1;
    _0x1c0.set(_arg2);
    _0x1b6 = false;
}

void Bird::initGlideOff(const sead::Matrix34f* _arg1, const sead::Vector3f& _arg2, bool _arg3) {
    _0x1b8 = _arg1;
    _0x1c0.set(_arg2);
    _0x1b6 = _arg3;
}

void Bird::makeActorAlive() {
    al::LiveActor::makeActorAlive();
    al::setShadowMaskDropLength(this, 1000, "丸影");
}

void Bird::appear() {
    al::LiveActor::appear();
    al::setNerve(this, &Delay);
}

static void startFlyAway(Bird* bird, const sead::Vector3f& horizontal_dir) {
    al::invalidateHitSensors(bird);
    al::validateHitSensorMapObjAll(bird);
    al::setFront(bird, horizontal_dir);
    al::setNerve(bird, &NrvBird.FlyAway);
}

static void startFlyAwayByRail(Bird* bird, const sead::Vector3f& horizontal_dir) {
    if (al::isExistRail(bird))
        al::setNerve(bird, &NrvBird.FlyRail);
    else
        startFlyAway(bird, horizontal_dir);
}

static void startFlyAwayFrom(Bird* bird, const sead::Vector3f& pos) {
    sead::Vector3f dir = al::getTrans(bird) - pos;
    al::verticalizeVec(&dir, al::getGravity(bird), dir);
    if (!al::tryNormalizeOrZero(&dir))
        dir = -al::getFront(bird);
    startFlyAway(bird, dir);
}

static void startFlyAwayFromByRail(Bird* bird, al::HitSensor* other) {
    if (al::isExistRail(bird))
        al::setNerve(bird, &NrvBird.FlyRail);
    else
        startFlyAwayFrom(bird, al::getActorTrans(other));
}

bool Bird::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    if (al::isNerve(this, &NrvBird.Freeze))
        return false;
    if (rs::isMsgKillByHomeDemo(message)) {
        makeActorDead();
        return true;
    }
    if (rs::isMsgBirdFlyAway(message)) {
        startFlyAwayFromByRail(this, other);
        return true;
    }
    if (al::isMsgPlayerDisregard(message))
        return al::isSensorNpc(self);
    if (rs::isMsgPlayerDisregardHomingAttack(message))
        return !mItemGenerator || mItemGenerator->isNone();
    if (al::isNerve(this, &NrvBird.FlyAway))
        return false;
    if (al::isSensorNpc(self)) {
        if (rs::isMsgShibakenApproach(message)) {
            startFlyAwayFromByRail(this, other);
            return true;
        }
        if ((rs::isMsgNpcCapReactionAll(message) && !rs::isMsgCapReflect(message)) ||
            rs::isMsgByugoBlow(message) || rs::isMsgSphinxRideAttack(message)) {
            startFlyAwayFromByRail(this, other);
            return false;
        }
        return false;
    }
    if (al::isSensorMapObj(self)) {
        if (mItemGenerator && rs::isMsgItemGetAll(message)) {
            sead::Vector3f shine_offset = {0, 120, 0};
            if ((mItemGenerator->isShine() &&
                 mItemGenerator->tryGenerate(al::getTrans(this) + shine_offset, al::getFront(this),
                                             1)) ||
                mItemGenerator->tryGenerate(al::getTrans(this), al::getFront(this), 1)) {
                al::startHitReaction(this, "ヒット");
                al::deleteEffect(this, "ShineHideGlowBird");
                mStateFlyAway->startFlyAwayByTouchAir();
                al::setNerve(this, &NrvBird.FlyAway);
                return true;
            }
        }
        if (al::isMsgNpcTouch(message)) {
            startFlyAwayFromByRail(this, other);
            return true;
        }
        return false;
    }
    return false;
}

void Bird::control() {
    if (mItemGenerator)
        mItemGenerator->tryUpdateHintTransIfExistShine();
    if (mWaterSurfaceShadow) {
        f32 height = 0;
        sead::Vector3f unused = {0, 0, 0};
        rs::calcGroundHeight(&height, &unused, this, al::getTrans(this), -al::getGravity(this), 0,
                             2500);
        mWaterSurfaceShadow->update(al::getTrans(this), -al::getGravity(this), height);
    }
}

void Bird::updateCollider() {
    if (al::isNerve(this, &NrvBird.GatheringFallDown) ||
        al::isNerve(this, &NrvBird.GatheringFallDownSlow)) {
        sead::Vector3f hit_pos = {0, 0, 0};
        al::Triangle triangle;
        if (alCollisionUtil::getFirstPolyOnArrow(this, &hit_pos, &triangle, al::getTrans(this),
                                                 al::getVelocity(this), nullptr, nullptr)) {
            if (al::isFloorPolygon(triangle.getNormal(0), al::getGravity(this))) {
                resetHostAnimInfo(mHostAnimInfo, triangle.getCollisionParts());
                al::setTrans(this, hit_pos);
                al::setVelocityZero(this);
                if (al::isExistAction(this, "Land"))
                    al::setNerve(this, &NrvBird.GatheringLand);
                else
                    al::setNerve(this, &NrvBird.Wait);
                return;
            }
            if (al::isWallPolygon(triangle.getNormal(0), al::getGravity(this))) {
                sead::Vector3f dir = triangle.getNormal(0);
                al::verticalizeVec(&dir, al::getFront(this), dir);
                al::verticalizeVec(&dir, al::getGravity(this), dir);
                if (!al::tryNormalizeOrZero(&dir))
                    dir = -al::getFront(this);
                startFlyAway(this, dir);
                return;
            }
        }
    }
    al::LiveActor::updateCollider();
}

void Bird::startFlyAwayHomeLanding() {
    mStateFlyAway->startFlyAwayByHomeLanding();
    startFlyAway(this, mInitialFront);
}

void Bird::startFlyAwayKoopa() {
    al::LiveActor::appear();
    mStateFlyAway->startFlyAwayByKoopa();
    startFlyAway(this, mInitialFront);
}

void Bird::startFlyAwayKoopa(const sead::Vector3f& horizontal_dir) {
    al::LiveActor::appear();
    mStateFlyAway->startFlyAwayByKoopa();
    startFlyAway(this, horizontal_dir);
}

void Bird::tryStartFlyAwayTRex(const sead::Vector3f& pos) {
    if (al::isNerve(this, &NrvBird.Wait))
        startFlyAwayFrom(this, pos);
}

void Bird::exeFreeze() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Freeze");
        al::setActionFrame(this, al::getActionFrameMax(this, al::getActionName(this)));
        al::setActionFrameRate(this, 0);
    }
}

void Bird::exeDelay() {
    if (al::isFirstStep(this))
        al::startAction(this, mStateWait ? mStateWait->getDefaultWaitActionName() : "GroundWaitA");
    rs::tryConnectToCollisionTFSV(this, mMtxConnector, &_0x140);
    if (al::isGreaterEqualStep(this, 12))
        startFlyAway(this, mInitialFront);
}

extern "C" bool f_0x37e144(Bird* bird, const BirdStateFlyAwayBase* _x1, bool _w2);

static bool f_0x37d454(Bird* bird, const Bird::Struct_0x108* info,
                       const BirdStateFlyAwayBase* state_fly_away,
                       const sead::Vector3f& initial_front, s32* fly_away_by_switch_timer, bool _w5,
                       const al::MtxConnector* mtx_connector, sead::Quatf* _x7) {
    rs::tryConnectToCollisionTFSV(bird, mtx_connector, _x7);
    if (rs::isActiveDemo(bird))
        return false;
    if (mtx_connector && !al::isMtxConnectorConnecting(mtx_connector)) {
        startFlyAwayByRail(bird, initial_front);
        return true;
    }
    if (al::isOnStageSwitch(bird, "SwitchFlyAway") && --*fly_away_by_switch_timer <= 0) {
        *fly_away_by_switch_timer = info->flyAwayBySwitchDelayStep;
        startFlyAwayByRail(bird, initial_front);
        return true;
    }
    return f_0x37e144(bird, state_fly_away, _w5);
}

static bool isMoved(Bird::HostAnimInfo* info, const al::MtxConnector* mtx_connector) {
    if (!info->collisionParts)
        return false;
    if (al::isSklAnimExist(info->collisionParts->getConnectedHost())) {
        if (info->sklAnimTime <= 1) {
            info->sklAnimName =
                al::isSklAnimPlaying(info->collisionParts->getConnectedHost(), 0) ?
                    al::getPlayingSklAnimName(info->collisionParts->getConnectedHost(), 0) :
                    nullptr;
        } else {
            if (!info->sklAnimName &&
                al::isSklAnimPlaying(info->collisionParts->getConnectedHost(), 0))
                return true;
            if (info->sklAnimName && !al::isSklAnimPlaying(info->collisionParts->getConnectedHost(),
                                                           info->sklAnimName, 0))
                return true;
        }
        info->sklAnimTime++;
    }
    return !mtx_connector && !al::isNear(al::getTrans(info->collisionParts->getConnectedHost()),
                                         info->initialTrans, 10);
}

static void clearHostAnimInfo(Bird::HostAnimInfo* info) {
    info->collisionParts = nullptr;
    info->sklAnimName = nullptr;
    info->sklAnimTime = -1;
}

static void startFlyAwayFromPlayerByRail(Bird* bird) {
    if (al::isExistRail(bird))
        al::setNerve(bird, &NrvBird.FlyRail);
    else
        startFlyAwayFrom(bird, rs::getPlayerPos(bird));
}

static bool flyAwayIfMoved(Bird* bird, Bird::HostAnimInfo* info,
                           const al::MtxConnector* mtx_connector) {
    if (!isMoved(info, mtx_connector))
        return false;
    clearHostAnimInfo(info);
    startFlyAwayFromPlayerByRail(bird);
    return true;
}

static const f32 gWaitShadowMaskLength[] = {175, 65, 1000};

void Bird::exeWait() {
    if (al::isFirstStep(this)) {
        _0x1ac = 0;
        al::setShadowMaskDropLength(this, gWaitShadowMaskLength[mWaitShadowMaskLengthType], "丸影");
    }
    if (f_0x37d454(this, _0x108, mStateFlyAway, mInitialFront, &mFlyAwayBySwitchTimer, _0x1b6,
                   mMtxConnector, &_0x140) ||
        flyAwayIfMoved(this, mHostAnimInfo, mMtxConnector))
        return;
    if (_0x1b8)
        rotateAndTranslate(al::getTransPtr(this), *_0x1b8, _0x1c0);
    al::updateNerveState(this);
}

void Bird::endWait() {
    al::setShadowMaskDropLength(this, 1000, "丸影");
}

void Bird::exeFlyRailGoal() {
    al::scaleVelocity(this, 0.9f);
    if (_0x108->waitStepAtRailEdge >= 0)
        al::setNerveAtGreaterEqualStep(this, &NrvBird.FlyAway, _0x108->waitStepAtRailEdge);
}

void Bird::exeFlyAway() {
    if (al::isFirstStep(this))
        al::invalidateClipping(this);
    if (!al::updateNerveState(this))
        return;
    if (mItemGenerator && mItemGenerator->isShine() && mItemGenerator->isEnableGenerateByCount(1)) {
        if (al::isJudgedToClipFrustum(this, mInitialTrans, al::getClippingRadius(this), 300)) {
            al::setVelocityZero(this);
            al::setFront(this, mInitialFront);
            al::resetPosition(this, mInitialTrans);
            if (al::isExistRail(this))
                al::setRailPosToStart(this);
            al::setNerve(this, mInitialNerve);
        }
    } else if (mIsLoop && !_0x1b8) {
        if (al::isJudgedToClipFrustum(this, mInitialTrans, 300, 300)) {
            al::resetPosition(this, mInitialTrans);
            al::setVelocityZero(this);
            al::setNerve(this, mInitialNerve);
        } else {
            al::invalidateClipping(this);
            al::hideModelIfShow(this);
            al::setNerve(this, &Reset);
        }
    } else {
        kill();
    }
}

void Bird::endFlyAway() {
    al::validateHitSensors(this);
    al::restartDitherAnimAutoCtrl(this);
    al::setModelAlphaMask(this, 1);
}

void Bird::exeReset() {
    if (al::isFirstStep(this))
        al::setVelocityZero(this);
    if (al::isJudgedToClipFrustum(this, mInitialTrans, 300, 300)) {
        al::resetPosition(this, mInitialTrans);
        al::showModelIfHide(this);
        al::setNerve(this, mInitialNerve);
    }
}

void Bird::endGatheringFallDown() {
    al::setModelAlphaMask(this, 1);
}

void Bird::exeGatheringLand() {
    if (al::isFirstStep(this))
        al::startAction(this, "Land");
    if (f_0x37d454(this, _0x108, mStateFlyAway, mInitialFront, &mFlyAwayBySwitchTimer, _0x1b6,
                   mMtxConnector, &_0x140) ||
        flyAwayIfMoved(this, mHostAnimInfo, mMtxConnector))
        return;
    if (_0x1b8)
        rotateAndTranslate(al::getTransPtr(this), *_0x1b8, _0x1c0);
    al::setNerveAtActionEnd(this, &NrvBird.Wait);
}

Bird* Bird::createForKoopaLv1(const al::ActorInitInfo& info) {
    Bird* bird = new Bird("白鳩");
    bird->mArchiveName = "BirdCloud";
    al::initLinksActor(bird, info, "Bird", 0);
    al::invalidateClipping(bird);
    bird->makeActorDead();
    return bird;
}

Bird* Bird::createForGatheringSpot(const char* name, const al::ActorInitInfo& info,
                                   const char* archive_name) {
    Bird* bird = new Bird(name);
    bird->mArchiveName = archive_name;
    bird->_0x1b5 = true;
    al::initCreateActorNoPlacementInfo(bird, info);
    bird->makeActorDead();
    return bird;
}

void Bird::appearGatheringSpot(const sead::Vector3f& start_pos, const sead::Vector3f& destination,
                               const sead::Vector3f& horizontal_dir) {
    al::LiveActor::appear();
    al::setModelAlphaMask(this, 0);
    al::invalidateClipping(this);
    _0x190.set(start_pos);
    _0x19c.set(destination);
    if (al::getRandom() < 0.15f)
        al::tryStartMtpAnimIfExist(this, "Pattern1");
    else
        al::tryStartMtpAnimIfExist(this, "Normal");
    al::resetPosition(this, start_pos);
    al::setFront(this, horizontal_dir);
    al::setNerve(this, &NrvBird.GatheringFallDown);
}

void Bird::forceFlyAwayGatheringSpot() {
    if (al::isNerve(this, &NrvBird.FlyAway))
        return;
    al::invalidateClipping(this);
    al::setNerve(this, &NrvBird.FlyAway);
}

bool Bird::isWait() const {
    return al::isNerve(this, &NrvBird.Wait);
}

Bird* Bird::createBirdGlideDownUfo(const char* name, const al::ActorInitInfo& info) {
    Bird* bird = new Bird(name);
    bird->mArchiveName = "BirdMoon";
    al::initCreateActorNoPlacementInfo(bird, info);
    bird->makeActorDead();
    return bird;
}

void Bird::tryStartGlideDown() {
    if (!al::isDead(this))
        return;
    sead::Vector3f destination;
    rotateAndTranslate(&destination, *_0x1b8, _0x1c0);
    f32 first_angle = al::getRandomRadian();
    for (s32 i = 0; i < 4; i++) {
        f32 angle = first_angle + 0.25f * i * sead::Mathf::pi();
        sead::Vector3f horizontal_dir = {sead::Mathf::sin(angle), 0, sead::Mathf::cos(angle)};
        sead::Vector3f start_pos =
            destination + (500 * -horizontal_dir + 3000 * sead::Vector3f::ey);
        if (!alCollisionUtil::getFirstPolyOnArrow(this, nullptr, nullptr, destination,
                                                  start_pos - destination, nullptr, nullptr)) {
            appearGatheringSpot(start_pos, destination, horizontal_dir);
            break;
        }
    }
}

void Bird::tryStartFlyAway() {
    if (al::isNerve(this, &NrvBird.Wait) || al::isNerve(this, &NrvBird.GatheringFallDown) ||
        al::isNerve(this, &NrvBird.GatheringFallDownSlow) ||
        al::isNerve(this, &NrvBird.GatheringLand)) {
        startFlyAwayFrom(this, rs::getPlayerPos(this));
    }
}
