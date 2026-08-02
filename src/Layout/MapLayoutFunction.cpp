#include "Layout/MapLayoutFunction.h"

#include "Library/Scene/SceneObjUtil.h"

#include "Layout/MapLayout.h"

namespace rs {

// TODO revisit these after implementing MapLayout
bool tryCalcMapNorthDir(sead::Vector3f*, const al::IUseSceneObjHolder* scene_obj_holder) {
    return false;
}

const sead::Matrix44f& getMapViewProjMtx(const al::IUseSceneObjHolder* scene_obj_holder) {
    auto ptr = reinterpret_cast<u8***>(al::getSceneObj<MapLayout>(scene_obj_holder));
    return *reinterpret_cast<sead::Matrix44f*>(ptr[0x2d][0x26]);
}

const sead::Matrix44f& getMapProjMtx(const al::IUseSceneObjHolder* scene_obj_holder) {
    auto ptr = reinterpret_cast<u8***>(al::getSceneObj<MapLayout>(scene_obj_holder));
    return *reinterpret_cast<sead::Matrix44f*>(ptr[0x2d][0x26] + 0x70);
}

}  // namespace rs

namespace StageMapFunction {

f32 getStageMapScaleMin() {
    return 0.3f;
}

f32 getStageMapScaleMax() {
    return 1.0f;
}

}  // namespace StageMapFunction
