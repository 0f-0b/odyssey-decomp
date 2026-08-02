#pragma once

#include "Library/Fluid/IUseFluidSurface.h"
#include "Library/LiveActor/LiveActor.h"

namespace al {
class FluidSimulateWave;
class HeightMapTextureBlender;
class PartsEffectGroup;
class SineWave;
class TextureReplacer;
}  // namespace al

class LavaWave : public al::LiveActor, public al::IUseFluidSurface {
public:
    LavaWave(const char* name);

    void init(const al::ActorInitInfo&) override;
    void initAfterPlacement() override;
    void control() override;

    void calcCoord(sead::Vector2i* out, const sead::Vector3f& pos, s32 texture_size) const;
    void startEndDemoBossMagma();
    void setEndDemoParamBossMagma(const sead::Vector3f& pos, s32 param);
    void endEndDemoBossMagma();
    void tryKillEmitterAndParticleAll();

    bool calcIsInArea(const sead::Vector3f& pos) const override;
    void calcPos(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    void calcPosFlat(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    void calcNormal(sead::Vector3f* out, const sead::Vector3f& pos) const override;
    bool tryAddRipple(const sead::Vector3f& pos, f32, f32) override;
    bool tryAddRippleWithRange(const sead::Vector3f& pos, f32, f32, f32 max_depth,
                               f32 max_height) override;
    bool tryAddQuadRipple(const sead::Vector3f& p1, const sead::Vector3f& p2,
                          const sead::Vector3f& p3, const sead::Vector3f& p4, f32) override;

    const char* getTypeName() const override { return "Fire"; }

private:
    al::SineWave* mSineWave = nullptr;
    al::FluidSimulateWave* mFluidSimulateWave = nullptr;
    al::HeightMapTextureBlender* mHeightMapTextureBlender = nullptr;
    al::TextureReplacer* mHeightTextureReplacer = nullptr;
    al::TextureReplacer* mNormalTextureReplacer = nullptr;
    s32 mTextureSize = 0;
    s32 mNrmTextureSize = 0;
    f32 mRadius = -1;
    sead::Vector4f mWaveParams = sead::Vector4f::zero;
    f32 _0x154 = 520;
    f32 mSimulateWaveViscosity = 5;
    f32 _0x15c = 0.995f;
    sead::Vector3f* mEffectPos = nullptr;
    s32* mEffectTimer = nullptr;
    f32* mEffectScale = nullptr;
    s32 mEffectIndex = 0;
    s32 mEmitEffectInterval = 12;
    f32 _0x180 = -250;
    f32 _0x184 = 150;
    s32 mEmitEffectTimer = 0;
    s32 mBossMagmaEndDemoRipple1st = 0;
    s32 mBossMagmaEndDemoRipple2nd = 0;
    s32 mBossMagmaEndDemoParam = -1;
    al::PartsEffectGroup* mEffectGroup = nullptr;
};

static_assert(sizeof(LavaWave) == 0x1a0);
