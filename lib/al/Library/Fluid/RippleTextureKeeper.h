#pragma once

#include <basis/seadTypes.h>
#include <container/seadPtrArray.h>

namespace agl {
class DrawContext;
}

namespace al {
class FluidSimulateWave;

class RippleTextureKeeper {
public:
    RippleTextureKeeper();
    s32 registerRippleTex(FluidSimulateWave*);
    void activateSampler(agl::DrawContext*, s32) const;

private:
    sead::FixedPtrArray<u8, 32> mTextures;
};

static_assert(sizeof(RippleTextureKeeper) == 0x110);

}  // namespace al
