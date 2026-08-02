#include "Library/Draw/SubCameraRenderer.h"

#include <common/aglTextureData.h>
#include <postfx/aglColorCorrection.h>

#include "Library/Camera/CameraViewInfo.h"
#include "Library/Camera/SceneCameraInfo.h"
#include "Library/Memory/HeapUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Graphics/OrthoRenderer.h"
#include "Library/Shader/Block/UniformBlockUtil.h"

namespace {
using namespace al;

NERVE_IMPL(SubCameraRenderer, CaptureFinish);
NERVE_IMPL(SubCameraRenderer, Capture);

NERVES_MAKE_STRUCT(SubCameraRenderer, CaptureFinish, Capture);
}  // namespace

namespace al {

// 0x1920e78
const UniformBlockLayout cLayout0[] = {{0, agl::UniformBlock::Type::_6, 1}};

SubCameraRenderer::SubCameraRenderer(agl::DrawContext* draw_context,
                                     GraphicsSystemInfo* graphics_system_info,
                                     ExecuteDirector* execute_director,
                                     SceneCameraInfo* scene_camera_info)
    : NerveExecutor("Subカメラ描画"), mDrawContext(draw_context),
      mExecuteDirector(execute_director), mSceneCameraInfo(scene_camera_info),
      mGraphicsSystemInfo(graphics_system_info) {}

SubCameraRenderer::~SubCameraRenderer() {
    if (mOrthoRenderer) {
        delete mOrthoRenderer;
        mOrthoRenderer = nullptr;
    }

    if (_0x28) {
        delete _0x28;
        _0x28 = nullptr;
    }
    if (_0x30) {
        delete _0x30;
        _0x30 = nullptr;
    }

    if (mColorCorrection) {
        delete mColorCorrection;
        mColorCorrection = nullptr;
    }

    if (_0x70.isValid()) {
        _0x70.deleteGPUMemBlock();
        _0x70.invalidate();
    }
    if (_0x88.isValid()) {
        _0x88.deleteGPUMemBlock();
        _0x88.invalidate();
    }
    if (_0xa0.isValid()) {
        _0xa0.deleteGPUMemBlock();
        _0xa0.invalidate();
    }
    if (_0xb8.isValid()) {
        _0xb8.deleteGPUMemBlock();
        _0xb8.invalidate();
    }
    if (_0xd0.isValid()) {
        _0xd0.deleteGPUMemBlock();
        _0xd0.invalidate();
    }
    if (_0xe8.isValid()) {
        _0xe8.deleteGPUMemBlock();
        _0xe8.invalidate();
    }
    if (_0x100.isValid()) {
        _0x100.deleteGPUMemBlock();
        _0x100.invalidate();
    }

    if (_0x38) {
        delete _0x38;
        _0x38 = nullptr;
    }
    if (_0x40) {
        delete _0x40;
        _0x40 = nullptr;
    }
    if (_0x48) {
        delete _0x48;
        _0x48 = nullptr;
    }
    if (_0x50) {
        delete _0x50;
        _0x50 = nullptr;
    }
    if (_0x58) {
        delete _0x58;
        _0x58 = nullptr;
    }
    if (_0x60) {
        delete _0x60;
        _0x60 = nullptr;
    }
    if (_0x68) {
        delete _0x68;
        _0x68 = nullptr;
    }
}

template <typename T>
agl::GPUMemBlock<T>* allocMemoryBlock(u64 size, sead::Heap* heap, s32 alignment,
                                      agl::MemoryAttribute attribute) {
    agl::GPUMemBlock<T>* block = new (heap) agl::GPUMemBlock<T>;
    block->allocBuffer_(size, heap, alignment, attribute);
    return block;
}

static void createColorTexture(agl::TextureData** texture_data, agl::GPUMemAddr<u8>* mem_addr,
                               const std::pair<s32, s32>& size) {
    if (*texture_data)
        return;
    *texture_data = new agl::TextureData;
    (*texture_data)
        ->initialize_(agl::TextureType::cTextureType_2D,
                      agl::TextureFormat::cTextureFormat_R8_G8_B8_A8_uNorm, size.first, size.second,
                      1, 1, agl::TextureAttribute::_0, agl::MultiSampleType::_0, true);

    agl::GPUMemBlock<u8>* block = allocMemoryBlock<u8>(
        reinterpret_cast<u32*>(*texture_data)[0x10], getCurrentHeap(),
        reinterpret_cast<u32*>(*texture_data)[0xf], agl::MemoryAttribute::Default);

    *mem_addr = agl::GPUMemAddr<u8>{*block, 0};
    (*texture_data)->setImagePtr(*mem_addr, 0);
}

static void createDepthTexture(agl::TextureData** texture_data, agl::GPUMemAddr<u8>* mem_addr,
                               const std::pair<s32, s32>& size) {
    if (*texture_data)
        return;
    *texture_data = new agl::TextureData;
    (*texture_data)
        ->initialize_(agl::TextureType::cTextureType_2D,
                      agl::TextureFormat::cTextureFormat_Depth_16, size.first, size.second, 1, 1,
                      agl::TextureAttribute::_2, agl::MultiSampleType::_0, true);

    agl::GPUMemBlock<u8>* block = allocMemoryBlock<u8>(
        reinterpret_cast<u32*>(*texture_data)[0x10], getCurrentHeap(),
        reinterpret_cast<u32*>(*texture_data)[0xf], agl::MemoryAttribute::Default);

    *mem_addr = agl::GPUMemAddr<u8>{*block, 0};
    (*texture_data)->setImagePtr(*mem_addr, 0);
}

void SubCameraRenderer::endInit() {
    if (mNumCameraSub <= 0)
        return;
    mCameraSubAreaScreenInfos.allocBuffer(mNumCameraSub, nullptr);
    initNerve(&NrvSubCameraRenderer.CaptureFinish, 0);
    mOrthoRenderer = new OrthoRenderer(mGraphicsSystemInfo);
    _0x28 = createUniformBlock(cLayout0, 1, nullptr, 2);
    _0x30 = createUniformBlock(cLayout0, 1, nullptr, 2);
    createColorTexture(&_0x38, &_0x70, _0x118);
    createColorTexture(&_0x40, &_0x88, _0x118);
    createColorTexture(&_0x48, &_0xa0, _0x118);
    createColorTexture(&_0x50, &_0xb8, _0x118);
    createColorTexture(&_0x58, &_0xd0, _0x118);
    createColorTexture(&_0x60, &_0xe8, _0x118);
    createDepthTexture(&_0x68, &_0x100, _0x118);
    mColorCorrection = new agl::pfx::ColorCorrection;
    mColorCorrection->initialize(1, nullptr, true);
}

void SubCameraRenderer::tryCapture() {
    if (mNumCameraSub <= 0 || !isNerve(this, &NrvSubCameraRenderer.Capture))
        return;
    CameraViewInfo* view = mSceneCameraInfo->getViewAt(1);
    if (view && view->isValid()) {
        // TODO
    }
}

void SubCameraRenderer::update() {
    if (mNumCameraSub <= 0)
        return;
    updateNerve();
    {
        UniformBlockSetter setter{_0x30, 0};
        _0x30->setV4f(0, {1, 1, 1, 1});
    }
    {
        UniformBlockSetter setter{_0x28, 0};
        _0x28->setV4f(0, {0, 0, 0, 1});
    }
}

void SubCameraRenderer::exeCapture() {}

void SubCameraRenderer::exeCaptureFinish() {
    if (mNumCameraSub <= 0)
        return;
    CameraViewInfo* view = mSceneCameraInfo->getViewAt(1);
    if (view && view->isValid())
        setNerve(this, &NrvSubCameraRenderer.Capture);
}

bool SubCameraRenderer::isCaptureFinish() const {
    return isNerve(this, &NrvSubCameraRenderer.CaptureFinish);
}

CameraSubAreaScreenInfo*
SubCameraRenderer::findCameraSubAreaScreenInfo(const sead::Vector3f&) const {
    for (s32 i = 0; i < mCameraSubAreaScreenInfos.size(); i++) {
        // TODO
    }
    return nullptr;
}

void SubCameraRenderer::calcOnScreenPos(sead::Vector3f* out, const sead::Vector3f&) const {
    for (s32 i = 0; i < mCameraSubAreaScreenInfos.size(); i++) {
        // TODO
    }
}

}  // namespace al
