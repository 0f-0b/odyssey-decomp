#include "Event/EventFlowNodeCheckFlag.h"

#include "Library/Base/StringUtil.h"
#include "Library/Bgm/BgmLineFunction.h"
#include "Library/Event/EventFlowFunction.h"
#include "Library/Placement/PlacementFunction.h"

#include "System/CollectBgm.h"
#include "System/GameDataFunction.h"
#include "System/GameDataUtil.h"
#include "Util/NpcEventFlowUtil.h"
#include "Util/StageInputFunction.h"

EventFlowNodeCheckFlag::EventFlowNodeCheckFlag(const char* name) : al::EventFlowNode(name) {}

void EventFlowNodeCheckFlag::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
}

static bool checkCompleteWorldTravelingPeach(const al::IUseSceneObjHolder* game_data_holder) {
    return rs::checkGetShineForWorldTravelingPeach(game_data_holder, "Moon");
}

s32 EventFlowNodeCheckFlag::getNextId() const {
    if (al::isEqualString(getName(), "CheckEnableUnlockHint"))
        return al::getCaseEventNextId(this, !GameDataFunction::isEnableUnlockHint(getActor()));
    if (al::isEqualString(getName(), "CheckGameClear"))
        return al::getCaseEventNextId(this, !GameDataFunction::isGameClear(getActor()));
    if (al::isEqualString(getName(), "CheckSeparatePlayMode"))
        return al::getCaseEventNextId(this, !rs::isSeparatePlay(getActor()));
    if (al::isEqualString(getName(), "CheckCollectedBgmCityWorldCelemony2D"))
        return al::getCaseEventNextId(this, !rs::isCollectedBgmCityWorldCelemony2D(getActor()));
    if (al::isEqualString(getName(), "CheckCompleteWorldTravelingPeach"))
        return al::getCaseEventNextId(this, !checkCompleteWorldTravelingPeach(getActor()));
    return al::getCaseEventNextId(this, 1);
}

EventFlowNodeCheckOpenDoorSnow::EventFlowNodeCheckOpenDoorSnow(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeCheckOpenDoorSnow::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
}

s32 EventFlowNodeCheckOpenDoorSnow::getNextId() const {
    if (rs::isEventAfterDoorSnow1(this))
        return al::getCaseEventNextId(this, 1);
    if (rs::isEventAfterDoorSnow2(this))
        return al::getCaseEventNextId(this, 2);
    if (rs::isEventAfterDoorSnow3(this))
        return al::getCaseEventNextId(this, 3);
    if (rs::isEventAfterDoorSnow4(this))
        return al::getCaseEventNextId(this, 4);
    return al::getCaseEventNextId(this, 0);
}

EventFlowNodeCheckPlayingCollectBgm::EventFlowNodeCheckPlayingCollectBgm(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeCheckPlayingCollectBgm::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
    al::tryGetStringArg(&mRequest, al::getActorInitInfo(info), "Request");
}

s32 EventFlowNodeCheckPlayingCollectBgm::getNextId() const {
    if (mRequest && al::isPlayingBgm(getActor(), "CollectBgm")) {
        if (const char* name = al::getCurPlayingBgmResourceName(getActor())) {
            if (CollectBgm::isEqualRequest(name, nullptr, mRequest))
                return al::getCaseEventNextId(this, 0);
            for (s32 i = 0; i < CollectBgm::cSituationNameListSize; i++) {
                const char* situation_name = CollectBgm::cSituationNameList[i];
                if (al::isStartedBgmSituation(getActor(), situation_name) &&
                    CollectBgm::isEqualRequest(name, situation_name, mRequest))
                    return al::getCaseEventNextId(this, 0);
            }
            return al::getCaseEventNextId(this, 1);
        }
    }
    return al::getCaseEventNextId(this, 2);
}

EventFlowNodeCheckFirstTalkCollectBgm::EventFlowNodeCheckFirstTalkCollectBgm(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeCheckFirstTalkCollectBgm::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
}

s32 EventFlowNodeCheckFirstTalkCollectBgm::getNextId() const {
    return al::getCaseEventNextId(this, !rs::isTalkCollectBgmNpc(getActor()));
}

EventFlowNodeFirstTalkEndCollectBgmNpc::EventFlowNodeFirstTalkEndCollectBgmNpc(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeFirstTalkEndCollectBgmNpc::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
}

void EventFlowNodeFirstTalkEndCollectBgmNpc::start() {
    rs::talkCollectBgmNpc(getActor());
    end();
}

EventFlowNodeCheckClear3CollectBgm::EventFlowNodeCheckClear3CollectBgm(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeCheckClear3CollectBgm::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
}

s32 EventFlowNodeCheckClear3CollectBgm::getNextId() const {
    return al::getCaseEventNextId(
        this, rs::calcGetShineNumCollectedBgm(getActor()) >= 3 &&
                  !GameDataFunction::isCollectedBgm(getActor(), CollectBgm::cBonusCollectBgmName01,
                                                    nullptr));
}

EventFlowNodeCheckCompleteCollectBgm::EventFlowNodeCheckCompleteCollectBgm(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeCheckCompleteCollectBgm::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
    al::initEventQuery(this, info);
}

s32 EventFlowNodeCheckCompleteCollectBgm::getNextId() const {
    return al::getCaseEventNextId(this, rs::checkGetShineCollectedBgmAll(getActor()));
}

EventFlowNodeGetCollectBgmBonus01::EventFlowNodeGetCollectBgmBonus01(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeGetCollectBgmBonus01::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
}

void EventFlowNodeGetCollectBgmBonus01::start() {
    GameDataFunction::trySetCollectedBgm(getActor(), CollectBgm::cBonusCollectBgmName01, nullptr);
    end();
}

EventFlowNodeGetCollectBgmBonus02::EventFlowNodeGetCollectBgmBonus02(const char* name)
    : al::EventFlowNode(name) {}

void EventFlowNodeGetCollectBgmBonus02::init(const al::EventFlowNodeInitInfo& info) {
    al::initEventFlowNode(this, info);
}

void EventFlowNodeGetCollectBgmBonus02::start() {
    GameDataFunction::trySetCollectedBgm(getActor(), CollectBgm::cBonusCollectBgmName02,
                                         CollectBgm::cBonusCollectBgmSituationName02);
    end();
}
