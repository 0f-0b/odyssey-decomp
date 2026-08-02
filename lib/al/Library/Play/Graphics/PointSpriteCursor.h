#pragma once

#include <gfx/seadColor.h>
#include <gfx/seadGraphicsContextMRT.h>
#include <math/seadVector.h>

namespace agl {
class ShaderProgram;
}

namespace al {
struct GraphicsRenderInfo;

class PointSpriteCursor {
public:
    enum class DrawType { _4 = 4, _5 = 5 };

    PointSpriteCursor(const char*, u64, const char*, DrawType);
    virtual ~PointSpriteCursor();
    void clear();
    void beginStore();
    void store(const sead::Vector3f&, f32, const sead::Color4f&, f32, f32, bool, bool);
    void endStore();
    void drawPointSprite(const GraphicsRenderInfo&, const agl::ShaderProgram*,
                         const sead::GraphicsContextMRT*) const;

private:
    void* _padding[0x18];
};

static_assert(sizeof(PointSpriteCursor) == 0xc8);

}  // namespace al
