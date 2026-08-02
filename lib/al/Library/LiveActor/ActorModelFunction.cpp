#include "Library/LiveActor/ActorModelFunction.h"

#include <nn/g3d/ModelObj.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Model/ModelCtrl.h"
#include "Library/Model/ModelFunction.h"
#include "Library/Model/ModelKeeper.h"

namespace al {

void getModelDrawCategoryFromShaderAssign(bool* deferred_opa, bool* deferred_xlu, bool* forward,
                                          bool* indirect, const LiveActor* actor) {
    *deferred_opa = false;
    *deferred_xlu = false;
    *forward = false;
    *indirect = false;
    nn::g3d::ModelObj* model_obj = actor->getModelKeeper()->getModelCtrl()->getModelObj();
    for (s32 i = 0; i < model_obj->GetNumShapes(); i++)
        if (alModelFunction::isShaderAssignDeferred(model_obj, i))
            if (alModelFunction::isShaderAssignXlu(model_obj, i))
                *deferred_xlu = true;
            else
                *deferred_opa = true;
        else if (alModelFunction::isShaderAssignIndirect(model_obj, i))
            *indirect = true;
        else
            *forward = true;
}

}  // namespace al
