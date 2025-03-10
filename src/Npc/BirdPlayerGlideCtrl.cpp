#include "Npc/BirdPlayerGlideCtrl.h"

#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/MtxUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Resource/ResourceHolder.h"
#include "Library/Scene/SceneObjUtil.h"
#include "Library/Yaml/ByamlUtil.h"

#include "Npc/Bird.h"
#include "System/GameDataFunction.h"
#include "Util/DemoUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/ScenePlayerCapFunction.h"

namespace {
NERVE_IMPL(BirdPlayerGlideCtrl, Invalid);
NERVE_IMPL(BirdPlayerGlideCtrl, ValidOnNose);
NERVE_IMPL(BirdPlayerGlideCtrl, WaitFlyAway);
NERVE_IMPL_(BirdPlayerGlideCtrl, ValidOnSitDownHeadNoCap, ValidOnSitDownHead);
NERVE_IMPL_(BirdPlayerGlideCtrl, ValidOnSitDownHeadWithCap, ValidOnSitDownHead);

NERVES_MAKE_STRUCT(BirdPlayerGlideCtrl, Invalid, ValidOnNose, WaitFlyAway, ValidOnSitDownHeadNoCap,
                   ValidOnSitDownHeadWithCap);
}  // namespace

static const sead::Vector3f gDefaultCapOffset = {0, -50, 0};

BirdPlayerGlideCtrl::BirdPlayerGlideCtrl(const char* name)
    : al::LiveActor(name), mCapOffset(gDefaultCapOffset) {}

static void setBirdScale(Bird* bird) {
    if (al::isEqualString(al::getModelName(bird), "BirdCloud") ||
        al::isEqualString(al::getModelName(bird), "BirdCity") ||
        al::isEqualString(al::getModelName(bird), "BirdSky"))
        al::setScaleAll(bird, 0.875f);
    else
        al::setScaleAll(bird, 0.7f);
}

void BirdPlayerGlideCtrl::init(const al::ActorInitInfo& info) {
    al::initActorSceneInfo(this, info);
    al::initActorPoseTRSV(this);
    al::initActorSRT(this, info);
    al::initActorClipping(this, info);
    al::invalidateClipping(this);
    al::initNerve(this, &NrvBirdPlayerGlideCtrl.Invalid, 0);
    al::initExecutorMapObjMovement(this, info);

    mCommonBird = new Bird(al::getLinksActorDisplayName(info, "Bird", 0));
    al::initLinksActor(mCommonBird, info, "Bird", 0);
    setBirdScale(mCommonBird);
    mCommonBird->initGlideOff(&mGlideOnMtx, sead::Vector3f::zero, true);
    mCommonBird->makeActorDead();

    mUfoBird = Bird::createBirdGlideDownUfo("月の鳥", info);
    setBirdScale(mUfoBird);
    mUfoBird->initGlideOff(&mGlideOnMtx, sead::Vector3f::zero, true);

    al::setSceneObj(this, this, 2);

    al::Resource* resource = al::findOrCreateResourceSystemData("BirdPlayerGlideInfo", nullptr);
    s32 node_size = sizeof(sead::StrTreeMap<32, sead::Vector3f>::Node);
    {
        al::ByamlIter iter{al::getByml(resource, "CapOffsetInfo")};

        if (s32 len = iter.getSize(); len > 0) {
            if (void* buf = sead::AllocBuffer(len * node_size, nullptr, 8))
                mCapOffsetInfo.setBuffer(len, buf);

            for (s32 i = 0; i < iter.getSize(); ++i) {
                al::ByamlIter subiter;
                al::getByamlIterByIndex(&subiter, iter, i);
                sead::Vector3f value = {0, 0, 0};
                al::tryGetByamlV3f(&value, subiter, "Offset");
                const char* name = al::getByamlKeyString(subiter, "CapName");
                mCapOffsetInfo.insert(name, value);
            }
        }
    }
    {
        al::ByamlIter iter{al::getByml(resource, "CostumeOffsetScaleInfo")};

        if (s32 len = iter.getSize(); len > 0) {
            if (void* buf = sead::AllocBuffer(len * node_size, nullptr, 8))
                mCostumeOffsetScaleInfo.setBuffer(len, buf);

            for (s32 i = 0; i < iter.getSize(); ++i) {
                al::ByamlIter subiter;
                al::getByamlIterByIndex(&subiter, iter, i);
                sead::Vector3f value = {0, 0, 0};
                al::tryGetByamlV3f(&value, subiter, "OffsetScale");
                const char* name = al::getByamlKeyString(subiter, "CostumeName");
                mCostumeOffsetScaleInfo.insert(name, value);
            }
        }
    }

    this->makeActorAlive();
}

void BirdPlayerGlideCtrl::initAfterPlacement() {
    if (reinterpret_cast<void**>(&mCapOffsetInfo)[2]) {
        const char* name = GameDataFunction::getCurrentCapTypeName(this);
        if (auto node = mCapOffsetInfo.find(name))
            std::memcpy(&mCapOffset, &node->value(), sizeof mCapOffset);
    }
    if (reinterpret_cast<void**>(&mCostumeOffsetScaleInfo)[2]) {
        const char* name = GameDataFunction::getCurrentCostumeTypeName(this);
        if (auto node = mCostumeOffsetScaleInfo.find(name))
            std::memcpy(&mCostumeOffsetScale, &node->value(), sizeof mCostumeOffsetScale);
    }
}

void BirdPlayerGlideCtrl::validateGlideOnNose() {
    mValidOnNose = true;
    if (al::isNerve(this, &NrvBirdPlayerGlideCtrl.Invalid))
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnNose);
    else if (!al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnNose) && mBird)
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
}

void BirdPlayerGlideCtrl::invalidateGlideOnNose() {
    mValidOnNose = false;
    if (mBird)
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
    else if (mValidOnSitDownHead)
        if (PlayerCapFunction::isEnableBirdLandPlayerCapOn(this))
            al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap);
        else
            al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadNoCap);
    else
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.Invalid);
}

void BirdPlayerGlideCtrl::validateGlideOnSitDownHead() {
    mValidOnSitDownHead = true;
    if (mValidOnNose)
        return;
    if (al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap) ||
        al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadNoCap))
        return;
    if (mBird)
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
    else if (PlayerCapFunction::isEnableBirdLandPlayerCapOn(this))
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap);
    else
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadNoCap);
}

void BirdPlayerGlideCtrl::invalidateGlideOnSitDownHead() {
    mValidOnSitDownHead = false;
    if (!(al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap) ||
          al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadNoCap)))
        return;
    if (mBird)
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
    else
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.Invalid);
}

void BirdPlayerGlideCtrl::addDemoActorAndFlyAway() {
    if (!mBird)
        return;
    rs::addDemoActor(this, false);
    rs::addDemoActor(mBird, false);
    al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
}

void BirdPlayerGlideCtrl::exeInvalid() {
    if (al::isFirstStep(this) && mBird && al::isAlive(mBird)) {
        mBird->tryStartFlyAway();
        mBird = nullptr;
    }
}

static bool tryCalcGlideOnNoseMtx(sead::Matrix34f* out, const al::LiveActor* actor) {
    sead::Matrix34f mtx = sead::Matrix34f::ident;
    if (!rs::tryCalcPlayerModelNoseJointMtx(&mtx, actor))
        return false;
    sead::Vector3f side;
    mtx.getBase(side, 0);
    sead::Vector3f up;
    mtx.getBase(up, 1);
    sead::Vector3f front;
    mtx.getBase(front, 2);
    sead::Vector3f pos;
    mtx.getBase(pos, 3);
    sead::Vector3f offset = {13, -2.5, 0};
    al::makeMtxFrontUpPos(out, front, up, pos + offset.x * side + offset.y * up + offset.z * front);
    return true;
}

void BirdPlayerGlideCtrl::exeValidOnNose() {
    if (al::isFirstStep(this))
        mGlideDownTime = al::getRandom(180, 1200);
    if (al::isGreaterEqualStep(this, mGlideDownTime)) {
        if (al::isStep(this, mGlideDownTime)) {
            if (!mBird) {
                mBird = GameDataFunction::isGameClear(this) && al::getRandom() < 0x1p-9 ?
                            mUfoBird :
                            mCommonBird;
            }
            if (al::isDead(mBird) && tryCalcGlideOnNoseMtx(&mGlideOnMtx, mBird))
                mBird->tryStartGlideDown();
        }
        if (al::isAlive(mBird))
            tryCalcGlideOnNoseMtx(&mGlideOnMtx, mBird);
    }
}

static bool tryCalcGlideOnSitDownHeadMtx(sead::Matrix34f* out, const al::LiveActor* actor,
                                         const sead::Vector3f& offset) {
    sead::Vector3f pos = {0, 0, 0};
    sead::Vector3f front = {0, 0, 0};
    sead::Vector3f up = {0, 0, 0};
    sead::Vector3f side = {0, 0, 0};
    if (!(rs::tryCalcPlayerModelHeadJointPos(&pos, actor) &&
          rs::tryCalcPlayerModelHeadJointFront(&front, actor) &&
          rs::tryCalcPlayerModelHeadJointUp(&up, actor) &&
          rs::tryCalcPlayerModelHeadJointSide(&side, actor)))
        return false;
    al::makeMtxFrontUpPos(out, front, up, pos + offset.x * side + offset.y * up + offset.z * front);
    return true;
}

static const sead::Vector3f gNoCapOffset = {0, -42.5, 0};

void BirdPlayerGlideCtrl::exeValidOnSitDownHead() {
    if (al::isFirstStep(this))
        mGlideDownTime = al::getRandom(180, 1200);
    if (al::isGreaterEqualStep(this, mGlideDownTime)) {
        sead::Vector3f cap_offset;
        if (al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap))
            cap_offset = mCapOffset;
        else
            cap_offset = gNoCapOffset;
        sead::Vector3f costume_offset_scale = mCostumeOffsetScale;
        sead::Vector3f offset = {cap_offset.x * costume_offset_scale.x,
                                 cap_offset.y * costume_offset_scale.y,
                                 cap_offset.z * costume_offset_scale.z};
        if (tryCalcGlideOnSitDownHeadMtx(&mGlideOnMtx, this, offset) &&
            al::isStep(this, mGlideDownTime)) {
            if (!mBird)
                mBird = mCommonBird;
            mBird->tryStartGlideDown();
        }
    }
    if (al::isNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap) !=
        PlayerCapFunction::isEnableBirdLandPlayerCapOn(this)) {
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.WaitFlyAway);
    }
}

void BirdPlayerGlideCtrl::exeWaitFlyAway() {
    if (mBird && !al::isDead(mBird))
        mBird->tryStartFlyAway();
    else if (mValidOnNose)
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnNose);
    else if (mValidOnSitDownHead)
        if (PlayerCapFunction::isEnableBirdLandPlayerCapOn(this))
            al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadWithCap);
        else
            al::setNerve(this, &NrvBirdPlayerGlideCtrl.ValidOnSitDownHeadNoCap);
    else
        al::setNerve(this, &NrvBirdPlayerGlideCtrl.Invalid);
}

namespace rs {

void validateGlideBirdOnPlayerNose(const al::LiveActor* player) {
    if (al::isExistSceneObj(player, 2)) {
        auto obj = static_cast<BirdPlayerGlideCtrl*>(al::getSceneObj(player, 2));
        obj->validateGlideOnNose();
    }
}

void invalidateGlideBirdOnPlayerNose(const al::LiveActor* player) {
    if (al::isExistSceneObj(player, 2)) {
        auto obj = static_cast<BirdPlayerGlideCtrl*>(al::getSceneObj(player, 2));
        obj->invalidateGlideOnNose();
    }
}

void validateGlideBirdOnSitDownPlayerHead(const al::LiveActor* player) {
    if (al::isExistSceneObj(player, 2)) {
        auto obj = static_cast<BirdPlayerGlideCtrl*>(al::getSceneObj(player, 2));
        obj->validateGlideOnSitDownHead();
    }
}

void invalidateGlideBirdOnSitDownPlayerHead(const al::LiveActor* player) {
    if (al::isExistSceneObj(player, 2)) {
        auto obj = static_cast<BirdPlayerGlideCtrl*>(al::getSceneObj(player, 2));
        obj->invalidateGlideOnSitDownHead();
    }
}

bool isPlayerSitDownChair(const Bird* bird) {
    if (al::isExistSceneObj(bird, 2)) {
        auto obj = static_cast<BirdPlayerGlideCtrl*>(al::getSceneObj(bird, 2));
        return obj->isValidOnSitDownHead();
    }
    return false;
}

}  // namespace rs

const char* BirdPlayerGlideCtrl::getSceneObjName() const {
    return getName();
}
