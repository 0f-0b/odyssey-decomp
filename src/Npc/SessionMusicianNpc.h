#pragma once

#include <container/seadPtrArray.h>
#include <prim/seadEnum.h>
#include <string>

#include "Library/Event/IEventFlowEventReceiver.h"
#include "Library/Event/IEventFlowQueryJudge.h"
#include "Library/LiveActor/LiveActor.h"

namespace al {
class EventFlowExecutor;
}
class BgmAnimeSynchronizer;
class CityManRhythmInfo;
class IUsePlayerPuppet;
class NpcJointLookAtController;
class NpcStateReaction;
class SessionMusicianWarpAgent;
class TalkNpcCap;
class TalkNpcParam;

class SessionMusicianNpc : public al::LiveActor,
                           public al::IEventFlowEventReceiver,
                           public al::IEventFlowQueryJudge {
public:
    using LiveActor::LiveActor;

    void init(const al::ActorInitInfo& info) override;
    void startEvent();
    void appear() override;
    void kill() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    bool receiveEvent(const al::EventFlowEventData* event) override;
    const char* judgeQuery(const char* judge_name) const override;
    void endClipped() override;
    void control() override;
    void forceControlForDance();
    void controlForDance();
    bool isJoined() const;
    bool isStateWarp() const;
    void doneWarp();
    bool isEnableMuteBgmTrack() const;

    void exeWaitNoEventFlowSabi();
    void exeWaitNoEventFlow();
    void exeWait();
    void exeWarpStart();
    void exeWarp();
    void exeWarpEnd();
    void exeReaction();
    void endReaction();

private:
    SEAD_ENUM(EventType, Wait, Live, Ceremony, PowerPlant)

    struct Struct_0x128 {
        sead::Vector3f offset;
        f32* distance;
        f32* horizontalAngle;
        f32* verticalAngle;
    };

    static_assert(sizeof(Struct_0x128) == 0x28);

    s32 mEventType = 0;
    al::EventFlowExecutor* mEventFlowExecutor = nullptr;
    Struct_0x128* _0x128 = nullptr;
    sead::PtrArray<al::LiveActor> mFans;
    SessionMusicianWarpAgent* mWarpAgent = nullptr;
    NpcStateReaction* mStateReaction = nullptr;
    TalkNpcParam* mTalkNpcParam = nullptr;
    NpcJointLookAtController* mJointLookAtController = nullptr;
    TalkNpcCap* mTalkNpcCap = nullptr;
    sead::Vector3f mMoonGetDemoTrans;
    sead::Quatf mMoonGetDemoQuat;
    s32 _0x184 = -1;
    IUsePlayerPuppet* _0x188 = nullptr;
    bool mIsJoined = false;
    std::string _0x198;
    std::string _0x1b0;
    CityManRhythmInfo* mRhythmInfo = nullptr;
    BgmAnimeSynchronizer* mBgmAnimeSynchronizer = nullptr;
    f32 _0x1d8 = -1;
    bool _0x1dc = true;
    bool mIsUseBgmTrackMute = false;
};

static_assert(sizeof(SessionMusicianNpc) == 0x1e0);
