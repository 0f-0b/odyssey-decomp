#pragma once

#include <container/seadPtrArray.h>

#include "Library/Layout/LayoutActor.h"

namespace al {
class LayoutInitInfo;
class PlayerHolder;
}  // namespace al

class MapMini : public al::LayoutActor {
public:
    MapMini(const al::LayoutInitInfo& info, const al::PlayerHolder* player_holder);

    void appearSlideIn();
    void end();
    bool isEnd() const;

    void exeAppear();
    void exeWait();
    void exeEnd();

    void calcNearHintTrans();

private:
    const al::PlayerHolder* mPlayerHolder;
    f32 _138 = 0;
    al::LayoutActor* mPlayerIcon = nullptr;
    sead::PtrArray<al::LayoutActor> mIcons;
    s32 mIconNumMax = 0;
};

static_assert(sizeof(MapMini) == 0x160);
