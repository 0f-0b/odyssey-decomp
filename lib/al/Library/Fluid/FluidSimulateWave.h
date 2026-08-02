#pragma once

#include <gfx/seadGraphicsContextMRT.h>
#include <math/seadQuat.h>
#include <prim/seadSafeString.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Play/Graphics/PartsGraphics.h"

namespace agl {
class DrawContext;
class RenderBuffer;
class TextureData;
class TextureSampler;
}  // namespace agl

namespace al {
class PointSpriteCursor;
class UniformBlock;

class FluidSimulateWave : public PartsGraphics, public HioNode {
public:
    FluidSimulateWave(GraphicsSystemInfo* info, s32 texture_size, f32, s32, bool, bool,
                      const char*);

    virtual ~FluidSimulateWave() { finalize(); }

    void beginStore();
    void initGaussian(s32);
    void finalize() override;
    void update(const GraphicsUpdateInfo& info) override;
    void calcGpu(const GraphicsCalcGpuInfo& info) override;
    void endStore();

    void drawSystem(const GraphicsRenderInfo* info) const override;
    void drawHeight(agl::DrawContext* ctx, agl::RenderBuffer* buffer) const;
    void drawNrm(agl::DrawContext* ctx, agl::RenderBuffer* buffer) const;
    void drawPoint(const GraphicsRenderInfo* info, agl::RenderBuffer* buffer) const;
    void storePoint(const sead::Vector2i& tex_coord, f32, f32, bool);
    void storePointWorld(const sead::Vector3f& pos, f32, f32, bool);
    void updateCenterPos(const sead::Vector3f& pos);
    void calcUboParam(sead::Vector4f* out) const;
    void tryCalcHeight(f32* out, const sead::Vector3f&) const;
    void getDisplacementMapTextureData() const;
    void getDisplacementMapTextureData(s32) const;
    void isDoubleBufDisp() const;

    const char* getName() const override { return _0x30.cstr(); }

    agl::TextureSampler* get_0x2d8() const { return (&_0x2d8)[1 - _0x328]; }

    void set_0x340(f32 _s0, f32 _s1, f32 _s2) {
        _0x340 = _s0;
        _0x344 = _s1;
        _0x348 = _s2;
    }

    void set_0x3a1() { _0x3a1 = true; }

private:
    struct Struct_0x380 {
        void* _padding[0x2];
    };

    s32 _0x28 = -1;
    sead::FixedSafeString<256> _0x30;
    PointSpriteCursor* _0x148 = nullptr;
    PointSpriteCursor* _0x150 = nullptr;
    s64 mShaderFluidSimulateWave;
    s64 mShaderHeightMapToNormal;
    s64 mShaderPointSprite2D;
    sead::GraphicsContextMRT _0x170;
    agl::TextureData* _0x298 = nullptr;
    agl::TextureData* _0x2a0 = nullptr;
    agl::TextureData* _0x2a8 = nullptr;
    agl::TextureData* _0x2b0 = nullptr;
    agl::TextureData* _0x2b8 = nullptr;
    agl::TextureData* _0x2c0 = nullptr;
    agl::TextureData* _0x2c8 = nullptr;
    agl::TextureData* _0x2d0 = nullptr;
    agl::TextureSampler* _0x2d8 = nullptr;
    agl::TextureSampler* _0x2e0 = nullptr;
    agl::TextureSampler* _0x2e8 = nullptr;
    agl::TextureSampler* _0x2f0 = nullptr;
    agl::TextureSampler* _0x2f8 = nullptr;
    agl::TextureSampler* _0x300 = nullptr;
    UniformBlock* _0x308 = nullptr;
    UniformBlock* _0x310 = nullptr;
    UniformBlock* _0x318 = nullptr;
    GraphicsSystemInfo* mGraphicsSystemInfo;
    s32 _0x328 = 0;
    s32 mTextureSize;
    f32 _0x330;
    f32 _0x334 = 1;
    f32 _0x338 = 0;
    f32 _0x33c = 0;
    f32 _0x340 = 400;
    f32 _0x344 = 5;
    f32 _0x348 = 0.997f;
    f32 _0x34c = 0;
    f32 _0x350 = 0;
    sead::Vector3f _0x354;
    sead::Vector3f _0x360;
    sead::Vector2f _0x36c = sead::Vector2f::zero;
    sead::Vector3f _0x374 = {0, 0, 0};
    sead::Quatf _0x380 = sead::Quatf::unit;
    sead::Vector3f _0x390 = sead::Vector3f::zero;
    bool _0x39c;
    bool _0x39d;
    bool _0x39e;
    bool _0x39f;
    bool _0x3a0;
    bool _0x3a1;
    s32 _0x3a4;
    s32 _0x3a8 = 0;
    bool _0x3ac;
    bool _0x3ad;
    f32 _0x3b0 = 0.7f;
    bool _0x3b4 = false;
    s32 _0x3b8 = 2;
    f32 _0x3bc = 10;
    sead::Vector3f _0x3c0 = {0, 0.01f, 0};
    sead::Vector3f _0x3cc = {0, 0, 0};
    sead::Vector3f _0x3d8 = {0, 0, 0};
    bool _0x3e4 = false;
    s32 _0x3e8 = 0;
};

static_assert(sizeof(FluidSimulateWave) == 0x3f0);

}  // namespace al
