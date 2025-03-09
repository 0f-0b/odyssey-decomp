#include "Player/PlayerJudgeSleep.h"

#include "Player/PlayerStateWait.h"

PlayerJudgeSleep::PlayerJudgeSleep(const PlayerStateWait* player_state_wait)
    : mPlayerStateWait(player_state_wait) {}

void PlayerJudgeSleep::reset() {}

void PlayerJudgeSleep::update() {}

bool PlayerJudgeSleep::judge() const {
    return mPlayerStateWait->isSleep();
}
