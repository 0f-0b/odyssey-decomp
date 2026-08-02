#include "Player/PlayerHackKeeper.h"

#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/KCollisionServer.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Scene/SceneObjUtil.h"

#include "Player/CapTargetInfo.h"
#include "Player/HackCap.h"
#include "Player/PlayerHackStartTexKeeper.h"
#include "System/GameDataFunction.h"

namespace {

class HackHostCollisionPartsFilter : public al::CollisionPartsFilterBase {
public:
    HackHostCollisionPartsFilter(const al::LiveActor* const* ptr) : mHackHostPtr(ptr) {}

    bool isInvalidParts(al::CollisionParts* parts) override {
        const al::HitSensor* sensor = parts->getConnectedSensor();
        return sensor && al::getSensorHost(sensor) == *mHackHostPtr;
    }

private:
    const al::LiveActor* const* mHackHostPtr;
};

}  // namespace

PlayerHackKeeper::PlayerHackKeeper(al::LiveActor* player, HackCap* cap,
                                   PlayerRecoverySafetyPoint* safety_point,
                                   const PlayerInput* input, const sead::Matrix34f* _x5,
                                   const PlayerDamageKeeper* damage_keeper,
                                   const IPlayerModelChanger* model_changer,
                                   const IUsePlayerHeightCheck* height_check)
    : mParent(player), mHackCap(cap), mRecoverySafePoint(safety_point), mInput(input), _30(_x5),
      mDamageKeeper(damage_keeper), mModelChanger(model_changer), mHeightCheck(height_check) {
    mCollisionFilter = new HackHostCollisionPartsFilter(&mHackActor);
    mHackStartTexKeeper = new PlayerHackStartTexKeeper;
    al::setSceneObj(mParent, mHackStartTexKeeper, SceneObjID_PlayerHackStartTexKeeper);
}

void PlayerHackKeeper::recordHack() {
    if (const char* name = mHackCap->getCapTargetInfo()->getHackName())
        GameDataFunction::addHackDictionary(mHackCap, name);
}

const IUsePlayerCollision* PlayerHackKeeper::getPlayerCollision() const {
    return mHackCap->getCapTargetInfo()->getPlayerCollision();
}

al::CollisionPartsFilterBase* PlayerHackKeeper::getCollisionPartsFilter() const {
    return mHackObjectInfo->isUseCollisionPartsFilterActor ? mCollisionFilter : nullptr;
}
