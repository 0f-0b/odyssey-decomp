#pragma once

namespace al {
class RenderVariables;
struct GraphicsCalcGpuInfo;
struct GraphicsComputeInfo;
struct GraphicsCopyInfo;
struct GraphicsRenderInfo;
struct GraphicsUpdateInfo;

class IUsePartsGraphics {
public:
    virtual void finalize() = 0;

    virtual void endInit() {}

    virtual void doCommandBufferCopy(const GraphicsCopyInfo* info) const {}

    virtual void doComputeShader(const GraphicsComputeInfo* info) const {}

    virtual void drawSystem(const GraphicsRenderInfo* info) const {}

    virtual void update(const GraphicsUpdateInfo& info) = 0;

    virtual void calcGpu(const GraphicsCalcGpuInfo& info) = 0;

    virtual void drawGBufferAfterSky(const GraphicsRenderInfo& info) const {}

    virtual void drawForward(const GraphicsRenderInfo& info, const RenderVariables& vars) const {}

    virtual void drawDeferred(const GraphicsRenderInfo& info) const {}

    virtual void drawLdr(const GraphicsRenderInfo& info) const {}

    virtual void drawIndirect(const GraphicsRenderInfo& info, const RenderVariables& vars) const {}

    virtual void drawCubemap(const GraphicsRenderInfo& info) const {}

    virtual const char* getName() const = 0;
};

}  // namespace al
