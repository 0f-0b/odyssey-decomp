#pragma once

#include <common/aglGPUMemAddr.h>
#include <container/seadPtrArray.h>
#include <gfx/seadCamera.h>
#include <gfx/seadProjection.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/Nerve/NerveExecutor.h"

namespace agl {
class DrawContext;
class RenderTargetDepth;
class TextureData;

namespace pfx {
class ColorCorrection;
}
}  // namespace agl

namespace al {
class AreaObj;
class ExecuteDirector;
class GraphicsSystemInfo;
class OrthoRenderer;
class SceneCameraInfo;
class UniformBlock;

struct CameraSubAreaScreenInfo {
    CameraSubAreaScreenInfo(const sead::Vector3f& scale, const sead::Matrix34f& mtx, AreaObj* obj)
        : screenScale(scale), screenMatrix(mtx), areaObj(obj) {}

    sead::Vector3f screenScale;
    sead::Matrix34f screenMatrix;
    AreaObj* areaObj;
};

static_assert(sizeof(CameraSubAreaScreenInfo) == 0x48);

class SubCameraRenderer : public NerveExecutor {
public:
    SubCameraRenderer(agl::DrawContext* draw_context, GraphicsSystemInfo* graphics_system_info,
                      ExecuteDirector* execute_director, SceneCameraInfo* scene_camera_info);
    ~SubCameraRenderer() override;

    void endInit();
    void draw(agl::DrawContext* draw_context, const agl::TextureData*, const agl::TextureData*,
              const agl::TextureData*, const agl::TextureData*, const agl::RenderTargetDepth*,
              bool);
    void tryCapture();
    void update();

    void exeCapture();
    void exeCaptureFinish();
    bool isCaptureFinish() const;
    CameraSubAreaScreenInfo* findCameraSubAreaScreenInfo(const sead::Vector3f&) const;
    void calcOnScreenPos(sead::Vector3f*, const sead::Vector3f&) const;

    void addCameraSub() { mNumCameraSub++; }

    void addCameraSubAreaScreenInfo(CameraSubAreaScreenInfo* screenInfo) {
        mCameraSubAreaScreenInfos.pushBack(screenInfo);
    }

private:
    agl::DrawContext* mDrawContext;
    OrthoRenderer* mOrthoRenderer = nullptr;
    ExecuteDirector* mExecuteDirector;
    UniformBlock* _0x28 = nullptr;
    UniformBlock* _0x30 = nullptr;
    agl::TextureData* _0x38 = nullptr;
    agl::TextureData* _0x40 = nullptr;
    agl::TextureData* _0x48 = nullptr;
    agl::TextureData* _0x50 = nullptr;
    agl::TextureData* _0x58 = nullptr;
    agl::TextureData* _0x60 = nullptr;
    agl::TextureData* _0x68 = nullptr;
    agl::GPUMemAddr<u8> _0x70;
    agl::GPUMemAddr<u8> _0x88;
    agl::GPUMemAddr<u8> _0xa0;
    agl::GPUMemAddr<u8> _0xb8;
    agl::GPUMemAddr<u8> _0xd0;
    agl::GPUMemAddr<u8> _0xe8;
    agl::GPUMemAddr<u8> _0x100;
    std::pair<s32, s32> _0x118 = {1024, 1024};
    sead::LookAtCamera _0x120;
    sead::OrthoProjection _0x180;
    SceneCameraInfo* mSceneCameraInfo;
    s32 mNumCameraSub = 0;
    GraphicsSystemInfo* mGraphicsSystemInfo;
    agl::pfx::ColorCorrection* mColorCorrection = nullptr;
    sead::PtrArray<CameraSubAreaScreenInfo> mCameraSubAreaScreenInfos;
};

static_assert(sizeof(SubCameraRenderer) == 0x260);

}  // namespace al
