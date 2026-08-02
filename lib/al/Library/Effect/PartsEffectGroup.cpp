#include "Library/Effect/PartsEffectGroup.h"

#include "Library/Effect/PartsEffect.h"

namespace al {

PartsEffectGroup::PartsEffectGroup() = default;

void PartsEffectGroup::init(s32 size, const EffectSystemInfo* systen_info, const char* _x3,
                            const sead::Vector3f* _x4, const sead::Vector3f* _x5,
                            const sead::Matrix34f* _x6, const IUseCamera* camera) {
    mEffects.allocBuffer(size, nullptr);
    for (s32 i = 0; i < size; i++) {
        PartsEffect* effect = new PartsEffect;
        effect->init(systen_info, _x3, _x4, _x5, _x6, camera);
        mEffects.pushBack(effect);
    }
}

PartsEffect* PartsEffectGroup::getEffect(s32 index) const {
    return mEffects[index];
}

s32 PartsEffectGroup::getEffectNum() const {
    return mEffects.size();
}

void PartsEffectGroup::update() {
    s32 size = getEffectNum();
    for (s32 i = 0; i < size; i++)
        getEffect(i)->update();
}

void PartsEffectGroup::kill() {
    s32 size = getEffectNum();
    for (s32 i = 0; i < size; i++)
        getEffect(i)->kill();
}

}  // namespace al
