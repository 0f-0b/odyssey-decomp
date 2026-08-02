#pragma once

#include "Library/Play/Graphics/PartsGraphics.h"

namespace agl {
class DrawContext;
}

namespace al {
class ShaderHolder;

class NoiseTextureKeeper : public PartsGraphics {
public:
    NoiseTextureKeeper(GraphicsSystemInfo*, ShaderHolder*);
    ~NoiseTextureKeeper();
    void finalize() override;
    void endInit() override;
    void declareUsingGemNoiseCubemapTexture();
    void declareUsingPerlinFbmNoise2DTexture();
    void declareUsingWorleyNoise2DTexture();
    void declareUsingFrostNoise2DTexture();
    void declareUsingCurlNoise2DTexture();
    void declareUsingPerlinNoise2DTexture();
    void declareUsingFrostNoise3DTexture();
    void declareUsingWorleyNoise3DTexture();
    void declareUsingWorleyThinNoise3DTexture();
    void declareUsingWorleyThinAnimNoise3DTexture();
    void declareUsingCloudLikeFbm3DNoiseTexture();
    void declareUsingOceanFoam3DNoiseTexture();
    void declareUsingSnowCovered3DNoiseTexture();
    void declareUsingCurlNoise3DTexture();
    void declareUsingRidgeNoise3DTexture();
    void declareUsingCaustics3DTexture();
    void declareUsingPerlinNoise3DTexture();
    void declareUsingPerlinFbmNoise3DTexture();
    void declareUsingGemNoise3DTexture();
    void declareUsingSimpleNoiseTexture();
    void declareUsingCloudNoise3DTexture();
    void getCloudVolume3DSampler() const;
    void activateGemNoiseCubemapTexture(agl::DrawContext*) const;
    void activateTexture2D(agl::DrawContext*, s32) const;
    void getTexture2DSampler(s32) const;
    void getTexture2D(s32) const;
    void getTexture3D(s32) const;
    void getTexture3DSampler(s32) const;
    void activateTexture3D(agl::DrawContext*, s32) const;
    void update(const GraphicsUpdateInfo&) override;
    void calcGpu(const GraphicsCalcGpuInfo&) override;
    void drawSystem(const GraphicsRenderInfo*) const override;

    virtual const char* getName() const override { return "ノイズテクスチャ管理"; }

private:
    void* _padding[0x44];
};

static_assert(sizeof(NoiseTextureKeeper) == 0x248);

}  // namespace al
