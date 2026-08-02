#pragma once

#include <basis/seadTypes.h>

#include "Library/Layout/LayoutActor.h"

namespace al {
class LayoutInitInfo;
}

class ShineChipLayoutParts : public al::LayoutActor {
public:
    ShineChipLayoutParts(const al::LayoutInitInfo&, const char*);

    void appear() override;
    void isInArea() const;
    void tryUpdateCount(s32, s32);
    void isEndCompleteAnim() const;

    void exeAppear();
    void exeWait();
    void exeAdd();
    void exeAddNoAnim();
    void exeComplete();
    void exeEnd();

private:
    void* _0x130;
    void* _0x138;
};

static_assert(sizeof(ShineChipLayoutParts) == 0x140);

namespace rs {
void tryUpdateShineChipLayoutCount(ShineChipLayoutParts*);
}
