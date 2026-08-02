#pragma once

namespace al {
class AreaObjGroup;
class FluidSurfaceHolder;

class NatureDirector {
public:
    NatureDirector();
    void init();

    void setWaterAreaGroup(AreaObjGroup* waterAreaGroup) { mWaterAreaGroup = waterAreaGroup; }

    FluidSurfaceHolder* getFluidSurfaceHolder() const { return mFluidSurfaceHolder; };

private:
    AreaObjGroup* mWaterAreaGroup;
    FluidSurfaceHolder* mFluidSurfaceHolder;
};

static_assert(sizeof(NatureDirector) == 0x10);

}  // namespace al
