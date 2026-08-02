#pragma once

#include "Library/Fluid/IUseFluidSurface.h"
#include "Library/Play/Graphics/PartsGraphics.h"

namespace agl {
class DrawContext;
class RenderBuffer;
class TextureData;
class TextureSampler;
}  // namespace agl

namespace al {

class SineWave : public PartsGraphics, public IUseFluidSurface {
public:
    SineWave(GraphicsSystemInfo* info, const sead::Vector2i& texture_size, const sead::Vector2f&);

    void finalize() override;
    void update(const GraphicsUpdateInfo&) override;
    void calcGpu(const GraphicsCalcGpuInfo&) override;
    void drawSystem(const GraphicsRenderInfo*) const override;
    void drawHeight(agl::DrawContext*, agl::RenderBuffer*) const;
    void drawNrm(agl::DrawContext*, agl::RenderBuffer*) const;
    void setWaveParams(f32 height, f32 width, f32 speed, f32 angle);

    bool calcIsInArea(const sead::Vector3f& pos) const override;
    void calcPos(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    void calcPosFlat(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    void calcNormal(sead::Vector3f* out, const sead::Vector3f& pos) const override;

    const char* getName() const override { return "サイン波"; }

    const char* getTypeName() const override { return mTypeName; }

    const agl::TextureData* get_0x40() const { return _0x40; }

    const agl::TextureData* get_0x48() const { return _0x48; }

    agl::TextureSampler* get_0x50() const { return _0x50; }

    void set_0x98() { _0x98 = true; }

    void set_0x9a() { _0x9a = true; }

private:
    void* _padding_0x30[0x2];
    const agl::TextureData* _0x40;
    const agl::TextureData* _0x48;
    agl::TextureSampler* _0x50;
    void* _padding_0x58[0x8];
    bool _0x98;
    bool _0x99;
    bool _0x9a;
    const char* mTypeName;
};

static_assert(sizeof(SineWave) == 0xa8);

}  // namespace al
