#pragma once

#include <gfx/seadCamera.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Play/Graphics/PartsGraphics.h"

namespace al {
class ExecuteDirector;

class OrthoRenderer {
public:
    OrthoRenderer(GraphicsSystemInfo*);
    ~OrthoRenderer();

    void updateView(const sead::LookAtCamera*, const sead::OrthoProjection*);

private:
    void* _padding[0x2];
};

static_assert(sizeof(OrthoRenderer) == 0x10);

class OrthoDepthTexture : public PartsGraphics, public HioNode {
public:
    OrthoDepthTexture(GraphicsSystemInfo*, ExecuteDirector*, const sead::Vector2f&);

    virtual ~OrthoDepthTexture() { finalize(); }

    void finalize() override;
    void update(const GraphicsUpdateInfo&) override;
    void drawSystem(const GraphicsRenderInfo*) const override;
    void setCameraXZ(f32, f32);
    void setCameraHeightFar(f32, f32);
    bool tryCalcDepth(f32*, const sead::Vector3f&) const;

    void calcGpu(const GraphicsCalcGpuInfo&) override {}

    const char* getName() const override { return "正射影デプス"; }

private:
    void* _padding[0x1b];
};

static_assert(sizeof(OrthoDepthTexture) == 0x100);

}  // namespace al
