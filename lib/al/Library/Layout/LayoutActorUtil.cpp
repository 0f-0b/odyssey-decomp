#include "Library/Layout/LayoutActorUtil.h"

#include <nn/ui2d/Layout.h>
#include <nn/ui2d/Pane.h>

#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutKeeper.h"

namespace al {

sead::Vector3f getPaneLocalTrans(const IUseLayout* layout, const char* pane_name) {
    const nn::util::Float3& pos = layout->getLayoutKeeper()
                                      ->getLayout()
                                      ->GetRootPane()
                                      ->FindPaneByName(pane_name, true)
                                      ->GetPosition();
    return {pos.x, pos.y, pos.z};
}

}  // namespace al
