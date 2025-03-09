#include "Library/Collision/PartsConnector.h"

#include "Library/LiveActor/ActorPoseUtil.h"

namespace al {

CollisionPartsConnector* createCollisionPartsConnector(const LiveActor* actor,
                                                       const sead::Quatf& quat) {
    CollisionPartsConnector* result = new CollisionPartsConnector;
    result->setBaseQuatTrans(quat, al::getTrans(actor));
    return result;
}

}  // namespace al
