#include "Library/Collision/Collider.h"

#include "Library/Math/MathUtil.h"

namespace al {
constexpr f32 Cos70 = 0.34202015f;

bool isWallPolygon(const sead::Vector3f& normal, const sead::Vector3f& gravity) {
    return calcPolygonType(normal, gravity) == PolygonType::Wall;
}

bool isFloorPolygon(const sead::Vector3f& normal, const sead::Vector3f& gravity) {
    return calcPolygonType(normal, gravity) == PolygonType::Floor;
}

static bool isWall(f32 dot) {
    return sead::Mathf::abs(dot) < Cos70;
}

static bool isFloor(f32 dot) {
    return !isWall(dot) && dot < 0;
}

bool isFloorPolygonCos(const sead::Vector3f& normal, const sead::Vector3f& gravity, f32 cos) {
    if (isNearZero(normal))
        return false;
    f32 dot = normal.dot(gravity);
    return !(-dot < cos) && isFloor(dot);
}

bool isCeilingPolygon(const sead::Vector3f& normal, const sead::Vector3f& gravity) {
    if (isNearZero(normal))
        return false;
    f32 dot = normal.dot(gravity);
    return !isWall(dot) && !isFloor(dot);
}

PolygonType calcPolygonType(const sead::Vector3f& normal, const sead::Vector3f& gravity) {
    if (isNearZero(normal))
        return PolygonType::None;
    f32 dot = normal.dot(gravity);
    return sead::Mathf::abs(dot) < Cos70 ? PolygonType::Wall :
           dot < 0                       ? PolygonType::Floor :
                                           PolygonType::Ceiling;
}

void calcTriangleColorByAngle(sead::Color4f* color, f32* angle_degrees, const sead::Vector3f& vec) {
    f32 angle = calcAngleDegree(vec, sead::Vector3f::ey);
    if (angle >= 60 && angle < 79.5)
        *color = sead::Color4f::cCyan;
    else if (angle >= 79.5 && angle <= 110)
        *color = sead::Color4f::cBlue;
    else if (angle > 110)
        *color = sead::Color4f::cMagenta;
    else
        *color = sead::Color4f::cGreen;
    if (angle_degrees)
        *angle_degrees = angle;
}

}  // namespace al
