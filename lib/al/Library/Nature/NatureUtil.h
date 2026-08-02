#pragma once

#include <basis/seadTypes.h>
#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class AreaObj;
class FireSurfaceFinder;
class IUseFluidSurface;
class LiveActor;
class NatureDirector;
class WaterSurfaceFinder;

void registerFluidSurfaceObj(IUseFluidSurface*, const LiveActor*);
bool isExistFluidSurface(const LiveActor*);
void setWaterRippleFieldScale(const LiveActor*, f32);
bool isInWaterPos(const LiveActor*, const sead::Vector3f&);
bool isInWater(const LiveActor*);
bool isInWaterNoIgnore(const LiveActor*, const sead::Vector3f&);
bool isInSodaWater(const LiveActor*, const sead::Vector3f&);
bool isInIceWaterPos(const LiveActor*, const sead::Vector3f&);
bool isInIceWater(const LiveActor*);
bool isInFirePos(const LiveActor*, const sead::Vector3f&);
bool isInFire(const LiveActor*);
bool isInCloudPos(const LiveActor*, const sead::Vector3f&);
bool isInCloud(const LiveActor*);
bool tryAddRippleMiddle(const LiveActor*);
bool tryAddRippleLarge(const LiveActor*);
bool isWaterAreaIgnore(const AreaObj*);
const char* getFireMaterialCode(const LiveActor*);

bool calcFindWaterSurface(sead::Vector3f*, sead::Vector3f*, const LiveActor*, const sead::Vector3f&,
                          const sead::Vector3f&, f32);
bool calcFindWaterSurfaceFlat(sead::Vector3f*, sead::Vector3f*, const LiveActor*,
                              const sead::Vector3f&, const sead::Vector3f&, f32);
bool calcFindWaterSurfaceDisplacement(sead::Vector3f*, sead::Vector3f*, const LiveActor*,
                                      const sead::Vector3f&, const sead::Vector3f&, f32);
bool calcFindWaterSurfaceOverGround(sead::Vector3f*, sead::Vector3f*, const LiveActor*,
                                    const sead::Vector3f&, const sead::Vector3f&, f32);
bool calcFindFireSurface(sead::Vector3f*, sead::Vector3f*, const LiveActor*, const sead::Vector3f&,
                         const sead::Vector3f&, f32);
bool calcFindCloudSurface(sead::Vector3f*, sead::Vector3f*, const LiveActor*, const sead::Vector3f&,
                          const sead::Vector3f&, f32);

bool tryAddRipple(const NatureDirector* director, const sead::Vector3f& pos, f32, f32);
bool tryAddRippleTiny(const LiveActor* actor);
bool tryAddRippleSmall(const LiveActor* actor, const sead::Vector3f& pos);
bool tryAddRippleTiny(const LiveActor* actor, const sead::Vector3f& pos);
bool tryAddRippleSmall(const LiveActor* actor);
bool tryAddRippleMiddle(const LiveActor* actor);
bool tryAddRippleMiddle(const LiveActor* actor, const sead::Vector3f& pos);
bool tryAddRippleLarge(const LiveActor* actor);
bool tryAddRippleLarge(const LiveActor* actor, const sead::Vector3f& pos);
bool tryAddRippleWithRange(const LiveActor* actor, const sead::Vector3f& pos, f32, f32, f32, f32);
bool tryAddRipple(const LiveActor* actor, const sead::Vector3f& pos, f32, f32);
bool tryAddRippleRandomBlur(const LiveActor* actor, const sead::Vector3f& pos, f32, f32, f32 blur);
bool tryAddQuadRipple(const LiveActor* actor, const sead::Vector3f& p1, const sead::Vector3f& p2,
                      const sead::Vector3f& p3, const sead::Vector3f& p4, f32);
bool tryAddQuadRipple(const LiveActor* actor, const sead::BoundBox3f& box,
                      const sead::Vector3f& pos, const sead::Quatf& quat, f32, f32 scale);
bool tryAddQuadRippleByBoxRotateY(const LiveActor* actor, const sead::BoundBox3f& box,
                                  const sead::Vector3f& pos, f32, f32 degrees, f32 scale);

void approachWaterSurfaceSpringDumper(LiveActor*, const WaterSurfaceFinder*, f32, f32, f32, f32,
                                      f32);
void approachFireSurfaceSpringDumper(LiveActor*, const FireSurfaceFinder*, f32, f32, f32, f32, f32);
void approachWaterSurfaceRate(LiveActor*, const WaterSurfaceFinder*, f32, f32, f32);
void approachFireSurfaceRate(LiveActor*, const FireSurfaceFinder*, f32, f32, f32);

void keepWaterSurfaceHeight(LiveActor*, const WaterSurfaceFinder*, f32);
void syncWaterSurfaceTrans(LiveActor*, const WaterSurfaceFinder*);
void syncFireSurfaceTrans(LiveActor*, const FireSurfaceFinder*);
void syncWaterSurfaceTransH(LiveActor*, const WaterSurfaceFinder*);
void blendWaterSurfaceTransH(LiveActor*, const WaterSurfaceFinder*, f32);
void syncWaterSurfaceUp(LiveActor*, const WaterSurfaceFinder*, f32);
void syncFireSurfaceUp(LiveActor*, const FireSurfaceFinder*, f32);
void calcMatrixFromActorPoseAndWaterSurfaceH(sead::Matrix34f*, const WaterSurfaceFinder*,
                                             const LiveActor*);

bool calcFindFireSurface(sead::Vector3f*, sead::Vector3f*, const LiveActor*, const sead::Vector3f&,
                         const sead::Vector3f&, f32);
}  // namespace al

namespace alNatureUtil {

bool calcFindDistanceWaterSurfaceToGround(f32*, const al::LiveActor*, const sead::Vector3f&, f32);

}  // namespace alNatureUtil
