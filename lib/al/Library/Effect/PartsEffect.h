#pragma once

#include <math/seadMatrix.h>

#include "Library/Effect/IUseEffectKeeper.h"

namespace al {
class EffectSystemInfo;
class IUseCamera;

class PartsEffect : public IUseEffectKeeper {
public:
    PartsEffect();
    void init(const EffectSystemInfo* system_info, const char*, const sead::Vector3f*,
              const sead::Vector3f*, const sead::Matrix34f*, const IUseCamera* camera);
    void update();
    void kill();

    EffectKeeper* getEffectKeeper() const override { return mEffectKeeper; }

private:
    EffectKeeper* mEffectKeeper = nullptr;
};

static_assert(sizeof(PartsEffect) == 0x10);

}  // namespace al
