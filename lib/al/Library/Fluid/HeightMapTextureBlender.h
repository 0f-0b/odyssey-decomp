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

class HeightMapTextureBlender : public PartsGraphics, public IUseFluidSurface {
public:
    HeightMapTextureBlender(GraphicsSystemInfo*, const sead::Vector2i&, const sead::Vector2i&,
                            const sead::Vector2f&);
    void setSource(agl::TextureSampler*, s32);

    void finalize() override;
    void update(const GraphicsUpdateInfo&) override;
    void calcGpu(const GraphicsCalcGpuInfo&) override;
    void drawSystem(const GraphicsRenderInfo*) const override;
    void drawHeight(agl::DrawContext*, agl::RenderBuffer*) const;
    void drawNrm(agl::DrawContext*, agl::RenderBuffer*) const;

    bool calcIsInArea(const sead::Vector3f& pos) const override;
    void calcPos(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    void calcNormal(sead::Vector3f* out, const sead::Vector3f& pos) const override;

    void calcPosFlat(sead::Vector3f* out, const sead::Vector3f& pos) const override {
        out->set(pos.x, _0x90, pos.z);
    }

    const char* getName() const override { return "Heightマップテクスチャ合成"; }

    const char* getTypeName() const override { return _0x98; }

    const agl::TextureData* get_0x48() const { return _0x48; }

    void set_0x8d() { _0x8d = true; }

private:
    void* _padding_0x30[0x3];
    const agl::TextureData* _0x48;
    void* _padding_0x50[0x7];
    s32 _0x88;
    bool _0x8c;
    bool _0x8d;
    f32 _0x90;
    const char* _0x98;
};

static_assert(sizeof(HeightMapTextureBlender) == 0xa0);

}  // namespace al
