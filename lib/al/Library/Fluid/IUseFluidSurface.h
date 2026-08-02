#pragma once

#include <math/seadVector.h>

namespace al {

class IUseFluidSurface {
public:
    virtual bool calcIsInArea(const sead::Vector3f& pos) const = 0;

    virtual void calcPos(sead::Vector3f* out, const sead::Vector3f& pos) const = 0;

    virtual void calcPosFlat(sead::Vector3f* out, const sead::Vector3f& pos) const {
        calcPos(out, pos);
    }

    virtual void calcDisplacementPos(sead::Vector3f* out, const sead::Vector3f& pos) const {
        calcPos(out, pos);
    }

    virtual void calcNormal(sead::Vector3f* out, const sead::Vector3f& pos) const = 0;

    virtual bool tryAddRipple(const sead::Vector3f& pos, f32, f32) { return false; }

    virtual bool tryAddRippleWithRange(const sead::Vector3f& pos, f32, f32, f32, f32) {
        return false;
    }

    virtual bool tryAddQuadRipple(const sead::Vector3f& p1, const sead::Vector3f& p2,
                                  const sead::Vector3f& p3, const sead::Vector3f& p4, f32) {
        return false;
    }

    virtual const char* getTypeName() const = 0;

    virtual void setFieldScale(f32 scale) {}
};

}  // namespace al
