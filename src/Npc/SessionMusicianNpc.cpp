#include "Npc/SessionMusicianNpc.h"

#include <sstream>

#include "Library/Demo/DemoFunction.h"
#include "Library/Event/EventFlowFunction.h"
#include "Library/Event/EventFlowUtil.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorParamHolder.h"
#include "Library/LiveActor/ActorParamHolderUtil.h"
#include "Library/LiveActor/ActorResourceFunction.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/LiveActor/LiveActorFunction.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Stage/StageSwitchUtil.h"
#include "Library/Thread/FunctorV0M.h"
#include "Library/Yaml/ByamlIter.h"

#include "Audio/BgmAnimeSynchronizer.h"
#include "Npc/CityManRhythmInfo.h"
#include "Npc/NpcStateReaction.h"
#include "Npc/SessionMusicianLocalFunction.h"
#include "Npc/SessionMusicianWarpAgent.h"
#include "Npc/TalkNpcCap.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/GameDataFunction.h"
#include "Util/NpcActionUtil.h"
#include "Util/NpcAnimUtil.h"
#include "Util/NpcEventFlowUtil.h"
#include "Util/PlayerPuppetFunction.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"

static const char* _0x1d67cc8[] = {"PlayAtFes1", "PlayAtFes2"};
static const char* _0x1d67cd8[] = {"PlayKimeAtFes1", "PlayKimeAtFes2", "PlayDrumActionB",
                                   "PlayGuitarActionB", "PlayTrumpetActionB"};
static const char* _0x1d68518[] = {"TalkDrum", "TalkBass", "TalkGuitar", "TalkTrumpet"};

namespace {
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, Wait);
NERVE_HOST_TYPE_END_IMPL(SessionMusicianNpc, Reaction);
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, WaitNoEventFlow);
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, WarpStart);
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, WaitNoEventFlowSabi);
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, WarpEnd);
NERVE_HOST_TYPE_IMPL(SessionMusicianNpc, Warp);

NERVES_MAKE_NOSTRUCT(HostType, WarpStart);
NERVES_MAKE_NOSTRUCT(HostType, WarpEnd);
NERVES_MAKE_NOSTRUCT(HostType, Warp);
NERVES_MAKE_STRUCT(HostType, Wait, Reaction, WaitNoEventFlow, WaitNoEventFlowSabi);
}  // namespace

static const char* f_0x3c3bcc(const al::ActorInitInfo&);

void SessionMusicianNpc::init(const al::ActorInitInfo& info) {
    al::initActorWithArchiveName(this, info, "BandMan", f_0x3c3bcc(info));
    al::initNerve(this, &NrvHostType.Wait, 8);
    al::tryGetLinksQT(&mMoonGetDemoQuat, &mMoonGetDemoTrans, info, "PlayerPosMoonGetDemo");
    mIsUseBgmTrackMute = al::tryGetBoolArgOrFalse(info, "IsUseBgmTrackMute");

    mStateReaction = NpcStateReaction::create(this, nullptr);
    mStateReaction->setParam(new NpcStateReactionParam("Reaction", "ReactionCap"));
    mStateReaction->set_0x29();
    al::initNerveState(this, mStateReaction, &NrvHostType.Reaction, "リアクション");

    mTalkNpcCap = TalkNpcCap::tryCreate(this, info);
    if (mTalkNpcCap) {
        al::registerSubActor(this, mTalkNpcCap);
        al::onSyncClippingSubActor(this, mTalkNpcCap);
        al::onSyncAppearSubActor(this, mTalkNpcCap);
        al::onSyncHideSubActor(this, mTalkNpcCap);
        al::onSyncAlphaMaskSubActor(this, mTalkNpcCap);
        al::startVisAnim(this, "DefaultHatOff");
    }

    al::tryGetArg(&mEventType, info, "EventType");

    if (const char* mtp_anim = nullptr; al::tryGetStringArg(&mtp_anim, info, "MtpAnim"))
        if (mtp_anim && !al::isEqualString(mtp_anim, "None"))
            al::tryStartMtpAnimIfExist(this, mtp_anim);

    if (const char* mcl_anim = nullptr; al::tryGetStringArg(&mcl_anim, info, "MclAnim"))
        if (mcl_anim && !al::isEqualString(mcl_anim, "None"))
            al::tryStartMclAnimIfExist(this, mcl_anim);

    if (mEventType == EventType::Wait || mEventType == EventType::PowerPlant)
        SessionMusicianLocalFunction::tryCreateSessionMusicianManager(this);

    SessionMusicianType type = SessionMusicianLocalFunction::getMusicianType(this);
    if (type == SessionMusicianType::Drum || type == SessionMusicianType::Bass ||
        type == SessionMusicianType::Guitar || type == SessionMusicianType::Trumpet) {
        _0x128 = new Struct_0x128{
            .offset = {al::findActorParamF32(this, "カメラ/オフセットX")->value,
                       al::findActorParamF32(this, "カメラ/オフセットY")->value,
                       al::findActorParamF32(this, "カメラ/オフセットZ")->value},
            .distance = &al::findActorParamF32(this, "カメラ/距離")->value,
            .horizontalAngle = &al::findActorParamF32(this, "カメラ/水平角度")->value,
            .verticalAngle = &al::findActorParamF32(this, "カメラ/垂直角度")->value,
        };
    }

    const char* param = f_0x3c3bcc(info);
    if (param && !(al::isEqualString(param, "Drum") || al::isEqualString(param, "Guitar") ||
                   al::isEqualString(param, "Bass") || al::isEqualString(param, "Trumpet"))) {
        param = nullptr;
    }
    mTalkNpcParam = rs::initTalkNpcParam(this, param);
    _0x198 = "PlayAtFes1";
    if (param) {
        std::stringstream ss;
        ss << "Play" << param << "Action";
        _0x1b0 = ss.str();
    }
    al::registActorToDemoInfo(this, info);
    al::initJointControllerKeeper(this, 1);
    mJointLookAtController = rs::tryCreateAndAppendNpcJointLookAtController(this, mTalkNpcParam);

    if (mEventType == EventType::Ceremony) {
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
        al::isExistModelResourceYaml(this, "DanceAnimInfo", nullptr);
        mRhythmInfo = new CityManRhythmInfo(
            this, al::getModelResourceYaml(this, "DanceAnimInfo", nullptr), false, 0);
    } else {
        if (al::isEqualString("Drum", f_0x3c3bcc(info) ? f_0x3c3bcc(info) : "")) {
            mEventFlowExecutor = rs::initEventFlowSuffix(this, info, "Drum", "SessionMusicianNpc",
                                                         EventType::text(mEventType));
        } else {
            mEventFlowExecutor =
                rs::initEventFlow(this, info, "SessionMusicianNpc", EventType::text(mEventType));
        }

        al::initEventReceiver(mEventFlowExecutor, this);
        rs::initEventParam(mEventFlowExecutor, mTalkNpcParam, nullptr);

        _0x184 = GameDataFunction::tryFindLinkedShineIndex(this, info);

        s32 num_fans = al::calcLinkChildNum(info, "Fan");
        if (num_fans > 0) {
            mFans.allocBuffer(num_fans, nullptr);

            for (s32 i = 0; i < num_fans; i++) {
                al::LiveActor* fan = al::createLinksActorFromFactory(info, "Fan", i);
                fan->makeActorDead();
                al::registActorToDemoInfo(fan, info);
                mFans.pushBack(fan);
            }
        }

        if (mEventType == EventType::Live) {
            rs::startEventFlow(mEventFlowExecutor, "Init");
        } else {
            SessionMusicianType type = SessionMusicianLocalFunction::getMusicianType(this);
            if (type == SessionMusicianType::Drum || type == SessionMusicianType::Bass ||
                type == SessionMusicianType::Guitar || type == SessionMusicianType::Trumpet) {
                if (rs::isDefinedEventCamera(mEventFlowExecutor, _0x1d68518[type])) {
                    rs::initEventCameraFixActorAutoAroundFront2(
                        mEventFlowExecutor, info, _0x1d68518[type], this, &_0x128->offset,
                        *_0x128->distance, *_0x128->horizontalAngle, *_0x128->verticalAngle);
                }
            }
            rs::initEventQueryJudge(mEventFlowExecutor, this);

            const char* flow = "Init";
            if (mEventType != EventType::Ceremony)
                if (mEventType == EventType::Wait &&
                    GameDataFunction::getSessionEventProgress(this) > SessionEventProgress::Entry)
                    flow = "Wait";
            rs::startEventFlow(mEventFlowExecutor, flow);

            mWarpAgent = new SessionMusicianWarpAgent(this, info);
            SessionMusicianLocalFunction::entryMusicianToManager(this);
            if (!SessionMusicianLocalFunction::isAlreadySessionMember(this) &&
                SessionMusicianLocalFunction::getMusicianType(this) != SessionMusicianType::Vocal) {
                for (s32 i = 0; i < mFans.size(); i++)
                    mFans[i]->appear();

                if (SessionMusicianLocalFunction::isMusicianType(this, SessionMusicianType::Drum))
                    // TODO
                    reinterpret_cast<bool*>(al::getSubActor(this, "ドラム"))[0x142] = false;

                using SessionMusicianNpcFunctor =
                    al::FunctorV0M<SessionMusicianNpc*, void (SessionMusicianNpc::*)()>;
                al::listenStageSwitchOnStart(
                    this, SessionMusicianNpcFunctor{this, &SessionMusicianNpc::startEvent});
                al::tryOnStageSwitch(this, "SwitchPlayingKeepOn");

                if (al::ByamlIter iter;
                    al::tryGetActorInitFileIter(&iter, this, "BgmRhythmSyncInfo", nullptr))
                    mBgmAnimeSynchronizer = BgmAnimeSynchronizer::tryCreate(this, iter);
            }
        }
    }

    makeActorAlive();
}

void SessionMusicianNpc::startEvent() {
    if (!al::isValidStageSwitch(this, "SwitchRouteGuideKeepOn"))
        return;
    if (!al::isOnStageSwitch(this, "SwitchRouteGuideKeepOn"))
        al::tryOnStageSwitch(this, "SwitchRouteGuideKeepOn");
}

void SessionMusicianNpc::appear() {
    al::LiveActor::appear();
    if (SessionMusicianLocalFunction::isMusicianType(this, SessionMusicianType::Vocal)) {
        al::hideModel(this);
        al::invalidateHitSensors(this);
    }
}

void SessionMusicianNpc::kill() {
    if (mEventType == EventType::Wait)
        for (s32 i = 0; i < mFans.size(); i++)
            mFans[i]->kill();
    al::tryOffStageSwitch(this, "SwitchRouteGuideKeepOn");
    al::tryOffStageSwitch(this, "SwitchPlayingKeepOn");
    al::LiveActor::kill();
}

void SessionMusicianNpc::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    rs::attackSensorNpcCommon(self, other);
}

bool SessionMusicianNpc::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                                    al::HitSensor* self) {
    if (rs::tryReceiveMsgPlayerDisregard(message, self, mTalkNpcParam))
        return true;
    if (rs::isMsgPlayerDisregardHomingAttack(message))
        return true;
    if (rs::isMsgPlayerDisregardTargetMarker(message))
        return true;
    if ((al::isNerve(this, &NrvHostType.Wait) || al::isNerve(this, &NrvHostType.WaitNoEventFlow) ||
         (al::isNerve(this, &NrvHostType.Reaction) && !al::isNewNerve(this))) &&
        (rs::isInvalidTrampleSensor(self, mTalkNpcParam) ?
             mStateReaction->receiveMsgWithoutTrample(message, other, self) :
             mStateReaction->receiveMsg(message, other, self))) {
        al::setNerve(this, &NrvHostType.Reaction);
        return true;
    }
    if (mStateReaction->receiveMsgNoReaction(message, other, self))
        return true;
    if (SessionMusicianLocalFunction::getMusicianType(this) == SessionMusicianType::Vocal) {
        if (al::isMsgBindStart(message))
            return true;
        if (al::isMsgBindInit(message)) {
            _0x188 = rs::startPuppet(self, other);
            return true;
        }
    }
    if (rs::isMsgCapAttack(message))
        return true;
    return false;
}

static void advanceProgress(al::LiveActor* actor) {
    SessionEventProgress next = SessionEventProgress::Entry;
    switch (GameDataFunction::getSessionEventProgress(actor)) {
    case SessionEventProgress::Wait1stMusician:
        next = SessionEventProgress::Wait2ndMusician;
        break;
    case SessionEventProgress::Wait2ndMusician:
        next = SessionEventProgress::Wait3rdMusician;
        break;
    case SessionEventProgress::Wait3rdMusician:
        next = SessionEventProgress::Wait4thMusician;
        break;
    case SessionEventProgress::Wait4thMusician:
        next = SessionEventProgress::RequestGoToThePowerPlant;
        break;
    case SessionEventProgress::WaitThePowerPlantWorks:
        next = SessionEventProgress::TheCeremonyIsReady;
        rs::setSceneStatusInvalidSave(actor);
        break;
    default:
        return;
    }
    GameDataFunction::setSessionEventProgress(actor, next);
}

bool SessionMusicianNpc::receiveEvent(const al::EventFlowEventData* event) {
    /*
        x29 = sp+0x30

        sp+0x18     GameDataHolderAccessor
        sp+0x10     SessionEventProgress
        sp+0x8      GameDataHolderAccessor
        sp          GameDataHolderAccessor
    */
    if (al::isEventName(event, "ShineGet")) {
        al::setNerve(this, &WarpStart);
        return true;
    }
    if (mEventType == EventType::Live && al::isEventName(event, "AppearFan")) {
        for (s32 i = 0; i < mFans.size(); i++)
            mFans[i]->appear();
        return true;
    }
    if (al::isEventName(event, "Joined")) {
        mIsJoined = true;
        advanceProgress(this);
        al::tryOffStageSwitch(this, "SwitchRouteGuideKeepOn");
        return true;
    }
    return false;
}

const char* SessionMusicianNpc::judgeQuery(const char* judge_name) const {
    if (al::isEqualString(judge_name, "JudgeMusicianType"))
        return SessionMusicianLocalFunction::getMusicianType(this).text();
    return nullptr;
}

void SessionMusicianNpc::endClipped() {
    _0x1dc = true;
    al::LiveActor::endClipped();
}

void SessionMusicianNpc::control() {
    if (mJointLookAtController)
        rs::updateNpcJointLookAtController(mJointLookAtController);
    if (mRhythmInfo) {
        mRhythmInfo->update(_0x1dc);
        if (mRhythmInfo->get_0x35() || _0x1dc) {
            _0x1dc = false;
            forceControlForDance();
        } else {
            controlForDance();
        }
        if (mBgmAnimeSynchronizer)
            mBgmAnimeSynchronizer->trySyncBgm();
    }
}

void SessionMusicianNpc::forceControlForDance() {
    s32 _w8 = mRhythmInfo->get_0x30();
    if (_w8 <= 0) {
        controlForDance();
        return;
    }
    s32 anim_id = mRhythmInfo->getAnimId(_w8 - 1);
    _0x1d8 = (mRhythmInfo->get_0xc() - mRhythmInfo->getAnimBeat(_w8 - 1)) * 3600 / 204;
    if (anim_id < 0)
        return;
    if (anim_id >= 2) {
        _0x1b0 = _0x1d67cd8[anim_id - 2];
        al::setNerve(this, &NrvHostType.WaitNoEventFlowSabi);
    } else {
        _0x198 = _0x1d67cc8[anim_id];
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
    }
}

void SessionMusicianNpc::controlForDance() {
    _0x1d8 = -1;
    s32 anim_id = mRhythmInfo->get_0x24();
    if (anim_id < 0)
        return;
    if (anim_id >= 2) {
        _0x1b0 = _0x1d67cd8[anim_id - 2];
        al::setNerve(this, &NrvHostType.WaitNoEventFlowSabi);
    } else {
        _0x198 = _0x1d67cc8[anim_id];
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
    }
}

bool SessionMusicianNpc::isJoined() const {
    return !al::isDead(this) && mIsJoined;
}

bool SessionMusicianNpc::isStateWarp() const {
    return !al::isNerve(this, &WarpEnd) && mIsJoined;
}

void SessionMusicianNpc::doneWarp() {
    al::setNerve(this, &WarpEnd);
}

bool SessionMusicianNpc::isEnableMuteBgmTrack() const {
    if (mEventType == EventType::Live)
        return true;
    if (mEventType == EventType::Ceremony && mIsUseBgmTrackMute)
        return true;
    return false;
}

void SessionMusicianNpc::exeWaitNoEventFlowSabi() {
    if (al::isFirstStep(this)) {
        al::startAction(this, _0x1b0.c_str());
        if (_0x1d8 > 0) {
            al::getActionFrameMax(this, _0x1b0.c_str());
            al::setActionFrame(this, _0x1d8);
        }
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
}

void SessionMusicianNpc::exeWaitNoEventFlow() {
    if (al::isFirstStep(this)) {
        al::validateClipping(this);
        al::startAction(this, _0x198.c_str());
        if (_0x1d8 > 0)
            al::setActionFrame(this, _0x1d8);
    }
}

void SessionMusicianNpc::exeWait() {
    if (al::isFirstStep(this))
        al::validateClipping(this);
    rs::updateEventFlow(mEventFlowExecutor);
}

void SessionMusicianNpc::exeWarpStart() {
    if (al::isFirstStep(this)) {
        if (SessionMusicianLocalFunction::getMusicianType(this) == SessionMusicianType::Vocal)
            rs::requestBindPlayer(this, al::getHitSensor(this, "Bind"));
        al::tryOffStageSwitch(this, "SwitchRouteGuideKeepOn");
        al::tryOffStageSwitch(this, "SwitchPlayingKeepOn");
    }
    if (SessionMusicianLocalFunction::getMusicianType(this) == SessionMusicianType::Vocal &&
        al::isLessStep(this, 0))
        return;
    if (mWarpAgent->tryStartWarp())
        al::setNerve(this, &Warp);
}

void SessionMusicianNpc::exeWarp() {}

void SessionMusicianNpc::exeWarpEnd() {
    kill();
}

static void f_0x3c4868(SessionMusicianNpc*, bool);

void SessionMusicianNpc::exeReaction() {
    if (al::isFirstStep(this)) {
        al::invalidateClipping(this);
        if (isEnableMuteBgmTrack())
            f_0x3c4868(this, true);
    }
    if (mEventFlowExecutor && rs::checkEnableStartEventAndCancelReaction(this, mTalkNpcParam)) {
        rs::updateEventFlow(mEventFlowExecutor);
        if (!al::isActionOneTime(this)) {
            al::setNerve(this, &NrvHostType.Wait);
            return;
        }
    }
    if (!al::updateNerveState(this))
        return;
    switch (mEventType) {
    case EventType::Live:
        al::startAction(this, "PlaySeOff");
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
        break;
    case EventType::Ceremony:
        al::startAction(this, _0x198.c_str());
        al::setNerve(this, &NrvHostType.WaitNoEventFlow);
        break;
    default:
        al::startAction(this, "PlaySeOn");
        al::setNerve(this, &NrvHostType.Wait);
        break;
    }
}

void SessionMusicianNpc::endReaction() {
    if (isEnableMuteBgmTrack())
        f_0x3c4868(this, false);
}
