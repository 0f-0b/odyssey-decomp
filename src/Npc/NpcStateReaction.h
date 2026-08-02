#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class LiveActor;
class SensorMsg;
class HitSensor;
}  // namespace al

class NpcStateReactionParam {
public:
    NpcStateReactionParam();
    NpcStateReactionParam(const char*, const char*);

private:
    void* _padding[0x16];
};

static_assert(sizeof(NpcStateReactionParam) == 0xb0);

class NpcStateReaction : public al::ActorStateBase {
public:
    static NpcStateReaction* create(al::LiveActor* actor, const NpcStateReactionParam* param);
    static NpcStateReaction* createForHuman(al::LiveActor* actor,
                                            const NpcStateReactionParam* param);
    static NpcStateReaction* createForHackActor(al::LiveActor* actor,
                                                const NpcStateReactionParam* param);

    void appear() override;
    void kill() override;
    virtual bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                            al::HitSensor* self);
    virtual bool receiveMsgWithoutTrample(const al::SensorMsg* message, al::HitSensor* other,
                                          al::HitSensor* self);
    virtual bool receiveMsgNoReaction(const al::SensorMsg* message, al::HitSensor* other,
                                      al::HitSensor* self);
    virtual bool isCapReaction() const;

    void exeReaction();
    void exeCapReaction();

    void setParam(NpcStateReactionParam* param) { mParam = param; }

    void set_0x29() { _0x29 = true; }

private:
    NpcStateReaction(al::LiveActor* actor, bool);

    const NpcStateReactionParam* mParam;
    bool _0x28 = false;
    bool _0x29 = false;
    bool _0x2a = false;
    bool _0x2b = false;
};

static_assert(sizeof(NpcStateReaction) == 0x30);
