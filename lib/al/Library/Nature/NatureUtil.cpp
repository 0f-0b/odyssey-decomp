#include "Library/Nature/NatureUtil.h"

#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureDirector.h"
#include "Project/Fluid/FluidSurfaceHolder.h"

namespace al {

bool tryAddRipple(const NatureDirector* director, const sead::Vector3f& pos, f32 _s0, f32 _s1) {
    return director->getFluidSurfaceHolder()->tryAddRippleAll(pos, _s0, _s1);
}

bool tryAddRippleTiny(const LiveActor* actor) {
    return tryAddRippleSmall(actor, getTrans(actor));
}

bool tryAddRippleSmall(const LiveActor* actor, const sead::Vector3f& pos) {
    return tryAddRipple(actor->getNatureDirector(), pos, 0.11f, 120);
}

bool tryAddRippleTiny(const LiveActor* actor, const sead::Vector3f& pos) {
    return tryAddRipple(actor->getNatureDirector(), pos, 0.05f, 100);
}

bool tryAddRippleSmall(const LiveActor* actor) {
    return tryAddRippleSmall(actor, getTrans(actor));
}

bool tryAddRippleMiddle(const LiveActor* actor) {
    return tryAddRippleMiddle(actor, getTrans(actor));
}

bool tryAddRippleMiddle(const LiveActor* actor, const sead::Vector3f& pos) {
    return tryAddRipple(actor->getNatureDirector(), pos, 0.4f, 170);
}

bool tryAddRippleLarge(const LiveActor* actor) {
    return tryAddRippleLarge(actor, getTrans(actor));
}

bool tryAddRippleLarge(const LiveActor* actor, const sead::Vector3f& pos) {
    return tryAddRipple(actor->getNatureDirector(), pos, 0.8f, 210);
}

bool tryAddRippleWithRange(const LiveActor* actor, const sead::Vector3f& pos, f32 _s0, f32 _s1,
                           f32 _s2, f32 _s3) {
    return actor->getNatureDirector()->getFluidSurfaceHolder()->tryAddRippleAllWithRange(
        pos, _s0, _s1, _s2, _s3);
}

bool tryAddRipple(const LiveActor* actor, const sead::Vector3f& pos, f32 _s0, f32 _s1) {
    return tryAddRipple(actor->getNatureDirector(), pos, _s0, _s1);
}

bool tryAddRippleRandomBlur(const LiveActor* actor, const sead::Vector3f& pos, f32 _s0, f32 _s1,
                            f32 blur) {
    sead::Vector3f offset;
    getRandomVector(&offset, blur);
    return tryAddRipple(actor->getNatureDirector(), pos + offset, _s0, _s1);
}

bool tryAddQuadRipple(const LiveActor* actor, const sead::Vector3f& p1, const sead::Vector3f& p2,
                      const sead::Vector3f& p3, const sead::Vector3f& p4, f32 _s0) {
    return actor->getNatureDirector()->getFluidSurfaceHolder()->tryAddQuadRippleAll(p1, p2, p3, p4,
                                                                                    _s0);
}

bool tryAddQuadRipple(const LiveActor* actor, const sead::BoundBox3f& box,
                      const sead::Vector3f& pos, const sead::Quatf& quat, f32 _s0, f32 scale) {
    sead::Vector3f quad[] = {
        box.getMin(),
        box.getMin() + sead::Vector3f{0, 0, box.getSizeZ()},
        box.getMin() + sead::Vector3f{box.getSizeX(), 0, box.getSizeZ()},
        box.getMin() + sead::Vector3f{box.getSizeX(), 0, 0},
    };
    for (sead::Vector3f& point : quad) {
        rotateVectorQuat(&point, quat);
        point += pos;
        lerpVec(&point, pos, point, scale);
    }
    return tryAddQuadRipple(actor, quad[0], quad[1], quad[2], quad[3], _s0);
}

bool tryAddQuadRippleByBoxRotateY(const LiveActor* actor, const sead::BoundBox3f& box,
                                  const sead::Vector3f& pos, f32 _s0, f32 degrees, f32 scale) {
    sead::Vector3f quad[] = {
        box.getMin(),
        box.getMin() + sead::Vector3f{0, 0, box.getSizeZ()},
        box.getMin() + sead::Vector3f{box.getSizeX(), 0, box.getSizeZ()},
        box.getMin() + sead::Vector3f{box.getSizeX(), 0, 0},
    };
    for (sead::Vector3f& point : quad) {
        point *= scale;
        point += pos;
        point.y = pos.y;
        rotateVectorCenterDegree(&point, point, sead::Vector3f::ey, pos, degrees);
    }
    return tryAddQuadRipple(actor, quad[0], quad[1], quad[2], quad[3], _s0);
}

}  // namespace al
