#pragma once

#include "Library/Layout/LayoutActor.h"

namespace al {
class LayoutInitInfo;
}

class ShineCounter : public al::LayoutActor {
public:
    ShineCounter(const char*, const al::LayoutInitInfo&);

    void kill() override;
    void tryStart();
    void tryStartWait();
    void tryEnd();
    void startCountAnim(bool);
    void isEndCountAnim() const;

    void exeAppear();
    void exeWait();
    void exeEnd();
    void exeShineCountAppear();
    void exeShineCountWait();
    void exeShineCountAdd();
};

static_assert(sizeof(ShineCounter) == 0x130);
