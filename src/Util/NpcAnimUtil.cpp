#include "Util/NpcAnimUtil.h"

#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveUtil.h"

namespace BirdFunction {

void tryUpdateFlyAwayDisappearDitherAlpha(al::LiveActor* actor, const al::IUseNerve* user,
                                          s32 start_step, s32 end_step) {
    if (!al::isGreaterEqualStep(user, start_step))
        return;
    if (al::isStep(user, start_step)) {
        al::stopDitherAnimAutoCtrl(actor);
        return;
    }
    f32 old_rate = 1 - sead::Mathf::clamp(
                           al::normalize(al::getNerveStep(user) - 1, start_step, end_step), 0, 1);
    f32 alpha_mask = old_rate <= 0.01f ? 0 : al::getModelAlphaMask(actor) / old_rate;
    f32 new_rate = 1 - al::calcNerveRate(user, start_step, end_step);
    al::setModelAlphaMask(actor, alpha_mask * new_rate);
}

}  // namespace BirdFunction
