#pragma once

#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>
#include <prim/seadSafeString.h>

namespace sead {
class Color4f;
}

namespace al {
class LiveActor;
struct ActorInitInfo;
class Resource;
class AudioKeeper;
}  // namespace al

class PlayerConst;
class PlayerCostumeInfo;
class PlayerJointControlFollowMtxPtr;
class PlayerJointControlPartsDynamics;
struct PlayerBodyCostumeInfo;

namespace PlayerFunction {
PlayerCostumeInfo* initMarioModelActor(al::LiveActor* player, const al::ActorInitInfo& initInfo,
                                       const char* modelName, const char* capType,
                                       al::AudioKeeper* keeper, bool isCloset);
void setupMarioFaceEarringVisibility(al::LiveActor*, const PlayerCostumeInfo*);
void setupMarioHeadStrapVisibility(al::LiveActor*, const PlayerCostumeInfo*);
void initMarioModelActorCloset(PlayerJointControlPartsDynamics**, al::LiveActor*,
                               const al::ActorInitInfo&, const char*, const char*,
                               const PlayerConst*, sead::Vector3f*, sead::Vector3f*,
                               PlayerJointControlFollowMtxPtr**, sead::Matrix34f*);
PlayerCostumeInfo* initMarioModelActorDemo(PlayerJointControlPartsDynamics** jointCtrlPtr,
                                           al::LiveActor* player, const al::ActorInitInfo& initInfo,
                                           const char* bodyName, const char* capName,
                                           const PlayerConst* pConst, sead::Vector3f* noseScale,
                                           sead::Vector3f* earScale, bool isCloset);
void initMarioModelActor2D(al::LiveActor* actor, const al::ActorInitInfo& initInfo,
                           const char* model2DName, bool isInvisCap);
void initYoshiModelActor(al::LiveActor*, const al::ActorInitInfo&, const char*);
void createCapModelName(sead::BufferedSafeString*, const char*);
al::Resource* initCapModelActor(al::LiveActor*, const al::ActorInitInfo&, const char*);
al::Resource* initCapModelActorDemo(al::LiveActor*, const al::ActorInitInfo&, const char*);
void initYoshiTongueParamHolder(al::LiveActor*);
bool isNeedHairControl(const PlayerBodyCostumeInfo*, const char*);
bool isInvisibleCap(const PlayerCostumeInfo*);
void showHairVisibility(al::LiveActor*);
void hideHairVisibility(al::LiveActor*);
void syncBodyHairVisibility(al::LiveActor*, al::LiveActor*);
void getMarioFaceNoseShrinkScale(sead::Vector3f*);
void getMarioFaceBigEarScale(sead::Vector3f*);
void syncMarioFaceBeardVisibility(al::LiveActor*, al::LiveActor*);
void setupMarioFaceBeardVisibility(al::LiveActor*, const PlayerCostumeInfo*);
void syncMarioHeadStrapVisibility(al::LiveActor*);
void setupClosetPlayerModel(al::LiveActor*, al::LiveActor*, al::LiveActor**, sead::Vector3f*,
                            sead::Vector3f*, sead::Matrix34f*, sead::Vector3f*, const char**,
                            sead::Matrix34f*, sead::Vector3f*, PlayerJointControlFollowMtxPtr*,
                            const PlayerCostumeInfo*);
void updateClosetHeadPartsMtx(al::LiveActor*, const al::LiveActor*, const sead::Matrix34f*,
                              const sead::Vector3f&, const char*, const sead::Matrix34f*,
                              const sead::Vector3f&, bool, sead::Matrix34f*);
PlayerConst* createMarioConst(const char*);
void createYoshiConst(al::LiveActor*, const char*, const char*);
void createPlayerDitherAnimator(al::LiveActor*, f32);
bool isPlayerHitPointOne(const al::LiveActor* actor);
bool isPlayerDeadStatus(const al::LiveActor* actor);
bool isPlayerDeadWipeStart(const al::LiveActor*);
bool isPlayerDeadEnableCoinAppear(const al::LiveActor*);
void getPlayerDeadWipeInfo(const al::LiveActor*, const char**, s32*);
bool isPlayerDeadDrawForward(const al::LiveActor*);
s32 getPlayerInputPort(const al::LiveActor*);
const sead::Matrix34f& getPlayerViewMtx(const al::LiveActor*);
void calcPlayerInputVec(sead::Vector3f*, const al::LiveActor*);
bool tryActivateAmiiboPreventDamage(const al::LiveActor*);
void getPlayerDepthGroundShadowName();
void changeDepthShadowMapSizeHigh(al::LiveActor*);
void changeDepthShadowMapSizeNormal(al::LiveActor*);
void createPlayerStainDecorationPartsArray(sead::PtrArray<sead::SafeString>*, al::LiveActor*);
void validatePlayerStain(al::LiveActor*, const sead::PtrArray<sead::SafeString>&);
void invalidatePlayerStain(al::LiveActor*, const sead::PtrArray<sead::SafeString>&);
void setupPlayerStain(al::LiveActor*, const sead::PtrArray<sead::SafeString>&, s32,
                      const sead::Color4f&, f32, f32, f32, f32);
}  // namespace PlayerFunction
