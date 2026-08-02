#pragma once

#include <basis/seadTypes.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;
}

struct CapType {
    const char* id;
    const char* fileName;
    const char* actorName;
};

class TalkNpcCap : public al::LiveActor {
public:
    TalkNpcCap(const CapType* cap_type) : al::LiveActor(cap_type->actorName), mCapType(cap_type) {}

    static TalkNpcCap* tryCreate(const al::LiveActor* actor, const al::ActorInitInfo& info);
    static TalkNpcCap* createForAchievementNpc(const al::LiveActor* actor,
                                               const al::ActorInitInfo& info);
    static TalkNpcCap* createForHintNpc(const al::LiveActor* actor, const al::ActorInitInfo& info);
    static TalkNpcCap* createForShibaken(const al::LiveActor* actor, const al::ActorInitInfo& info);
    static TalkNpcCap* createForShoppingNpc(const al::LiveActor* actor,
                                            const al::ActorInitInfo& info);
    static TalkNpcCap* createForShoppingNpcChromakey(const al::LiveActor* actor,
                                                     const al::ActorInitInfo& info);
    static TalkNpcCap* createForVolleyballNpc(const al::LiveActor* actor,
                                              const al::ActorInitInfo& info);

    void initAttach(const al::LiveActor* actor);
    void makeActorAlive() override;
    void control() override;
    void init(const al::ActorInitInfo& info) override;

private:
    const CapType* mCapType;
    const sead::Matrix34f* mMtx = nullptr;
    sead::Vector3f mLocalRotate = {0, 0, 0};
    sead::Vector3f mLocalTrans = {0, 0, 0};
    f32 mLocalScale = 1;
    bool mIsChromakey = false;
};

static_assert(sizeof(TalkNpcCap) == 0x138);
