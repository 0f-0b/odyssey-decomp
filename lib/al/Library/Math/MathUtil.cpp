#include "Library/Math/MathUtil.h"

#include <math/seadMathCalcCommon.h>
#include <random/seadGlobalRandom.h>

namespace al {
f32 normalize(f32 var, f32 min, f32 max) {
    if (sead::Mathf::abs(max - min) < 0.001f) {
        if (var < min)
            return 0.0f;
        else
            return 1.0f;
    }

    f32 clamped = sead::Mathf::clamp(var, min, max);
    return (clamped - min) / (max - min);
}

f32 normalize(s32 var, s32 min, s32 max) {
    s32 v3;
    f32 result;

    if (var <= min)
        return 0.0f;

    if (max - min <= 0) {
        result = 1.0f;

        if (var < min)
            result = 0.0f;
    } else {
        if (var <= max)
            v3 = var;
        else
            v3 = max;

        if (var < min)
            v3 = min;

        result = (static_cast<f32>(v3 - min) / static_cast<f32>(max - min));
    }

    return result;
}

f32 easeIn(f32 var) {
    return (((var * -0.5f) + 1.5f) * var) * var;
}

f32 easeOut(f32 var) {
    return (((var * -0.5f) * var) + 1.5f) * var;
}

f32 easeInOut(f32 var) {
    return (((var * -2.0f) + 3.0f) * var) * var;
}

f32 sign(f32 var) {
    if (var < 0.0f)
        return -1.0f;
    if (var > 0.0f)
        return 1.0f;

    return var;
}

s32 sign(s32 var) {
    if (var < 0)
        return -1;
    if (var > 0)
        return 1;

    return 0;
}

f32 squareIn(f32 var) {
    return var * var;
}

f32 squareOut(f32 var) {
    return (2.0f - var) * var;
}

f32 powerIn(f32 x, f32 y) {
    return powf(x, y);
}

f32 powerOut(f32 x, f32 y) {
    return powf(x, 1.0 / y);
}

f32 lerpValue(f32 x, f32 y, f32 time) {
    if (time < 0.0f)
        time = 0.0f;
    else if (time > 1.0f)
        time = 1.0f;
    return (x * (1.0f - time)) + (time * y);
}

void calcParabolicFunctionParam(f32* gravity, f32* initialVelY, f32 maxHeight,
                                f32 verticalDistance) {
    // can be 0, 1, or -1
    f32 signOfA3 = maxHeight < 0.0f ? -1.0f : maxHeight > 0.0f ? 1.0f : maxHeight;

    f32 maxHeightAdjusted =
        sead::Mathf::sqrt(sead::Mathf::clampMin((maxHeight - verticalDistance) * maxHeight, 0.0));
    *initialVelY = 2 * ((signOfA3 * maxHeightAdjusted) + maxHeight);
    *gravity = verticalDistance - *initialVelY;
}

f32 getRandom() {
    u32 random = (sead::GlobalRandom::instance()->getU32() >> 9) | 0x3F800000;
    return (*reinterpret_cast<f32*>(&random)) - 1;
}

f32 getRandom(f32 factor) {
    return getRandom(0.0f, factor);
}

f32 getRandom(f32 min, f32 max) {
    return (getRandom() * (max - min)) + min;
}

s32 getRandom(s32 factor) {
    return getRandom(0, factor);
}

s32 getRandom(s32 min, s32 max) {
    return (s32)getRandom((f32)min, (f32)max);
}

f32 getRandomDegree() {
    return getRandom(360.0f);
}

f32 getRandomRadian() {
    return getRandom(6.2832f);
}

void getRandomVector(sead::Vector3f* vec, f32 factor) {
    f32 x = (getRandom() * (factor + factor)) - factor;
    f32 y = (getRandom() * (factor + factor)) - factor;
    f32 z = (getRandom() * (factor + factor)) - factor;
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

void getRandomDir(sead::Vector3f* vec) {
    getRandomVector(vec, 10.0f);
    while (vec->dot(*vec) < 0.000001f) {
        *vec = {0.0f, 0.0f, 0.0f};
        getRandomVector(vec, 10.0f);
    }
    vec->normalize();
}

}  // namespace al
