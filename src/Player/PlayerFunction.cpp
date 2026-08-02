#include "Player/PlayerFunction.h"

#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/Player/PlayerUtil.h"

#include "Player/PlayerActorBase.h"
#include "Player/PlayerInfo.h"
#include "Player/PlayerJudgeDead.h"
#include "System/GameDataFunction.h"
#include "Util/JudgeUtil.h"

namespace PlayerFunction {

bool isPlayerHitPointOne(const al::LiveActor* actor) {
    return GameDataFunction::getPlayerHitPoint(actor) == 1;
}

bool isPlayerDeadStatus(const al::LiveActor* actor) {
    PlayerInfo* info = static_cast<PlayerActorBase*>(al::getPlayerActor(actor, 0))->getPlayerInfo();
    return info ? rs::isJudge(info->getJudgeDead()) : al::isDead(al::getPlayerActor(actor, 0));
}

}  // namespace PlayerFunction
