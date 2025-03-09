#include "Npc/Bird.h"

#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Shadow/ActorShadowUtil.h"

namespace {
NERVE_END_IMPL(Bird, Wait);
NERVE_IMPL(Bird, Freeze);
NERVE_END_IMPL(Bird, FlyAway);
NERVE_IMPL(Bird, FlyRail);
NERVE_IMPL(Bird, Delay);
NERVE_END_IMPL(Bird, GatheringFallDown);
NERVE_IMPL(Bird, GatheringFallDownSlow);
NERVE_IMPL(Bird, GatheringLand);
NERVE_IMPL(Bird, FlyRailGoal);
NERVE_IMPL(Bird, Reset);

[[gnu::used]] NERVES_MAKE_NOSTRUCT(Bird, Delay);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(Bird, Reset);
NERVES_MAKE_STRUCT(Bird, Wait, Freeze, FlyAway, FlyRail, GatheringFallDown, GatheringFallDownSlow,
                   GatheringLand, FlyRailGoal);
}  // namespace

Bird::Bird(const char* name) : al::LiveActor(name), _0x158(&NrvBird.Wait) {
    _0x108 = new SomeStruct;
}

void Bird::exeFreeze() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Freeze");
        al::setActionFrame(this, al::getActionFrameMax(this, al::getActionName(this)));
        al::setActionFrameRate(this, 0);
    }
}

void Bird::endWait() {
    al::setShadowMaskDropLength(this, 1000, "丸影");
}

void Bird::exeFlyRailGoal() {
    al::scaleVelocity(this, 0.9f);
    if (s32 step = _0x108->_0x4; step >= 0)
        al::setNerveAtGreaterEqualStep(this, &NrvBird.FlyAway, step);
}

void Bird::endFlyAway() {
    al::validateHitSensors(this);
    al::restartDitherAnimAutoCtrl(this);
    al::setModelAlphaMask(this, 1);
}

void Bird::exeReset() {
    if (al::isFirstStep(this))
        al::setVelocityZero(this);
    if (al::isJudgedToClipFrustum(this, _0x160, 300, 300)) {
        al::resetPosition(this, _0x160);
        al::showModelIfHide(this);
        al::setNerve(this, _0x158);
    }
}

void Bird::endGatheringFallDown() {
    al::setModelAlphaMask(this, 1);
}

void Bird::forceFlyAwayGatheringSpot() {
    if (al::isNerve(this, &NrvBird.FlyAway))
        return;
    al::invalidateClipping(this);
    al::setNerve(this, &NrvBird.FlyAway);
}

bool Bird::isWait() const {
    return al::isNerve(this, &NrvBird.Wait);
}
