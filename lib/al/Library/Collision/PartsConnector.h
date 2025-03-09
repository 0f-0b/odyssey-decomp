#pragma once

#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/Collision/CollisionPartsConnector.h"

namespace al {
class ActorInitInfo;
class CollisionParts;
class LiveActor;
class MtxConnector;

MtxConnector* createMtxConnector(const LiveActor* actor);
MtxConnector* createMtxConnector(const LiveActor* actor, const sead::Quatf&);
MtxConnector* tryCreateMtxConnector(const LiveActor* actor, const ActorInitInfo& info);
MtxConnector* tryCreateMtxConnector(const LiveActor* actor, const ActorInitInfo& info,
                                    const sead::Quatf&);
CollisionPartsConnector* createCollisionPartsConnector(const LiveActor* actor,
                                                       const sead::Quatf& quat);
CollisionPartsConnector* tryCreateCollisionPartsConnector(const al::LiveActor*,
                                                          const al::ActorInitInfo&);
CollisionPartsConnector* tryCreateCollisionPartsConnector(const al::LiveActor*,
                                                          const al::ActorInitInfo&,
                                                          const sead::Quatf&);
void attachMtxConnectorToCollision(MtxConnector* mtxConnector, const LiveActor* actor, bool);
void attachMtxConnectorToCollision(MtxConnector* mtxConnector, const LiveActor* actor,
                                   const sead::Vector3f&, const sead::Vector3f&);
CollisionParts* attachMtxConnectorToCollision(MtxConnector* mtxConnector, const LiveActor* actor,
                                              f32, f32);
void connectPoseQT(LiveActor* actor, const MtxConnector* mtxConnector, const sead::Quatf& quat,
                   const sead::Vector3f& trans);
void connectPoseQT(LiveActor* actor, const MtxConnector* mtxConnector);
void calcConnectTrans(sead::Vector3f*, const MtxConnector*, const sead::Vector3f&);
void attachMtxConnectorToJoint(MtxConnector* mtxConnector, const LiveActor* actor,
                               const char* jointName);
void attachMtxConnectorToJoint(MtxConnector* mtxConnector, const LiveActor* actor,
                               const char* jointName, const sead::Vector3f& quatInitVec,
                               const sead::Vector3f& trans);
void disconnectMtxConnector(MtxConnector* mtxConnector);
bool isMtxConnectorConnecting(const MtxConnector* mtxConnector);
}  // namespace al
