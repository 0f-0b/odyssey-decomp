#pragma once

#include "Player/IJudge.h"

class PlayerStateWait;

class PlayerJudgeSleep : public IJudge {
public:
    PlayerJudgeSleep(const PlayerStateWait* player_state_wait);
    void reset() override;
    void update() override;
    bool judge() const override;

private:
    const PlayerStateWait* mPlayerStateWait;
};
