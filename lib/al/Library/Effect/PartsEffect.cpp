#include "Library/Effect/PartsEffect.h"

#include "Library/Effect/EffectKeeper.h"

namespace al {

PartsEffect::PartsEffect() = default;

void PartsEffect::init(const EffectSystemInfo* system_info, const char* _x2,
                       const sead::Vector3f* _x3, const sead::Vector3f* _x4,
                       const sead::Matrix34f* _x5, const IUseCamera* camera) {
    mEffectKeeper = new EffectKeeper(system_info, _x2, _x3, _x4, _x5);
    alEffectKeeperInitFunction::setupCameraToEffectKeeper(mEffectKeeper, camera);
}

void PartsEffect::update() {
    mEffectKeeper->update();
}

void PartsEffect::kill() {
    getEffectKeeper()->deleteAndClearEffectAll();
}

}  // namespace al
