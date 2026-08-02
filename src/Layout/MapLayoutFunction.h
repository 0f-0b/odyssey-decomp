#pragma once

#include <math/seadMatrix.h>

namespace al {
class IUseSceneObjHolder;
}

namespace rs {
bool tryCalcMapNorthDir(sead::Vector3f* out, const al::IUseSceneObjHolder* scene_obj_holder);
const sead::Matrix44f& getMapViewProjMtx(const al::IUseSceneObjHolder* scene_obj_holder);
const sead::Matrix44f& getMapProjMtx(const al::IUseSceneObjHolder* scene_obj_holder);
void appearMapWithHint(const al::IUseSceneObjHolder* scene_obj_holder);
void addAmiiboHintToMap(const al::IUseSceneObjHolder* scene_obj_holder);
void appearMapWithAmiiboHint(const al::IUseSceneObjHolder* scene_obj_holder);
void appearMapMoonRockDemo(const al::IUseSceneObjHolder* scene_obj_holder, s32);
void endMap(const al::IUseSceneObjHolder* scene_obj_holder);
bool isEndMap(const al::IUseSceneObjHolder* scene_obj_holder);
bool isEnableCheckpointWarp(const al::IUseSceneObjHolder* scene_obj_holder);
}  // namespace rs

namespace StageMapFunction {
f32 getStageMapScaleMin();
f32 getStageMapScaleMax();
}  // namespace StageMapFunction
