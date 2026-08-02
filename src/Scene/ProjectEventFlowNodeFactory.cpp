#include "Scene/ProjectEventFlowNodeFactory.h"

#include "Library/Event/CreateEventFlowNode.h"
#include "Library/Event/EventFlowNodeActionLoop.h"
#include "Library/Event/EventFlowNodeJoin.h"

#include "Event/EventFlowNodeCheckFlag.h"

namespace al {
template <>
EventFlowNode* createEventFlowNode<void>(const char*) {
    return nullptr;
}

using EventFlowNodeActionOneTime = void;
using EventFlowNodeActorBaseMovementEnd = void;
using EventFlowNodeActorBaseMovementStart = void;
using EventFlowNodeActorKill = void;
using EventFlowNodeAnimCameraStart = void;
using EventFlowNodeCameraEnd = void;
using EventFlowNodeCameraStart = void;
using EventFlowNodeCheckSwitch = void;
using EventFlowNodeCheckWaitSwitch = void;
using EventFlowNodeDemoAction = void;
using EventFlowNodeDemoCamera = void;
using EventFlowNodeDemoPlayerAction = void;
using EventFlowNodeDemoPlayerHide = void;
using EventFlowNodeDemoPlayerShow = void;
using EventFlowNodeEvent = void;
using EventFlowNodeEventQuery = void;
using EventFlowNodeFork = void;
using EventFlowNodeHitReaction = void;
using EventFlowNodeJumpEntry = void;
using EventFlowNodeQueryJudge = void;
using EventFlowNodeSwitchOff = void;
using EventFlowNodeSwitchOn = void;
using EventFlowNodeTurnToPlayer = void;
using EventFlowNodeTurnToPlayerActionOneTime = void;
using EventFlowNodeTurnToPreDir = void;
}  // namespace al

using EventFlowNodeAmiiboTouchLayout = void;
using EventFlowNodeAppearMapAmiiboHint = void;
using EventFlowNodeBgmCtrl = void;
using EventFlowNodeBindKeepDemoStart = void;
using EventFlowNodeCapManHeroTalkAppear = void;
using EventFlowNodeCapManHeroTalkFocus = void;
using EventFlowNodeCapManHeroTalkPlayerTurn = void;
using EventFlowNodeCapManHeroTalkReturn = void;
using EventFlowNodeCapManHeroTalkSetDemoEndPose = void;
using EventFlowNodeCapManHeroTalkSetDemoStartPose = void;
using EventFlowNodeCapMessage = void;
using EventFlowNodeChangeStage = void;
using EventFlowNodeChangeWorldDemoMessage = void;
using EventFlowNodeCheckCostume = void;
using EventFlowNodeCheckCostumeInvisible = void;
using EventFlowNodeCheckCostumeMissMatchPart = void;
using EventFlowNodeCheckCostumePair = void;
using EventFlowNodeCheckCount = void;
using EventFlowNodeCheckGetLinkShine = void;
using EventFlowNodeCheckLifeUpItem = void;
using EventFlowNodeCheckLink = void;
using EventFlowNodeCheckMoonLockOpened = void;
using EventFlowNodeCheckPlayerOnGround = void;
using EventFlowNodeCheckYukimaruRaceResult = void;
using EventFlowNodeCloseTalkMessage = void;
using EventFlowNodeCoinPayment = void;
using EventFlowNodeCutSceneDemoStart = void;
using EventFlowNodeDemoEnd = void;
using EventFlowNodeDemoForceStart = void;
using EventFlowNodeDemoForceStartOnGround = void;
using EventFlowNodeDemoResetPlayerDynamics = void;
using EventFlowNodeDemoStart = void;
using EventFlowNodeDirectGetLinkShine = void;
using EventFlowNodeEnableHint = void;
using EventFlowNodeForcePutOnDemoCap = void;
using EventFlowNodeGetAmiiboCostume = void;
using EventFlowNodeGetAmiiboNotSearchHintNum = void;
using EventFlowNodeGetSearchAmiibo = void;
using EventFlowNodeIsCostumeAmiibo = void;
using EventFlowNodeIsEnableSearchAmiibo = void;
using EventFlowNodeIsTalkAmiiboHelp = void;
using EventFlowNodeKakkuTurn = void;
using EventFlowNodeMessageBalloon = void;
using EventFlowNodeMessageTalk = void;
using EventFlowNodeMessageTalkSpecialPurpose = void;
using EventFlowNodeNextTalkMessage = void;
using EventFlowNodeNormalDemoTryStart = void;
using EventFlowNodeNpcMoveToLink = void;
using EventFlowNodeOpenBgmList = void;
using EventFlowNodePlayerAction = void;
using EventFlowNodePlayerTurn = void;
using EventFlowNodePopItem = void;
using EventFlowNodePopLinkShine = void;
using EventFlowNodeReplacePlayer = void;
using EventFlowNodeSceneWipeClose = void;
using EventFlowNodeSelectChoice = void;
using EventFlowNodeSelectYesNo = void;
using EventFlowNodeSessionWaitMusician = void;
using EventFlowNodeSetDemoInfoDemoName = void;
using EventFlowNodeUnlockHint = void;
using EventFlowNodeWaitSimple = void;
using EventFlowNodeWaitWipeOpenEnd = void;
using EventFlowNodeWipeFadeBlackClose = void;
using EventFlowNodeWipeFadeBlackOpen = void;

const al::NameToCreator<al::EventFlowNodeCreatorFunction> sProjectEventFlowNodeFactoryEntries[] = {
    {"ActionLoop", al::createEventFlowNode<al::EventFlowNodeActionLoop>},
    {"ActionOneTime", al::createEventFlowNode<al::EventFlowNodeActionOneTime>},
    {"ActorBaseMovementEnd", al::createEventFlowNode<al::EventFlowNodeActorBaseMovementEnd>},
    {"ActorBaseMovementStart", al::createEventFlowNode<al::EventFlowNodeActorBaseMovementStart>},
    {"ActorKill", al::createEventFlowNode<al::EventFlowNodeActorKill>},
    {"AmiiboTouchLayout", al::createEventFlowNode<EventFlowNodeAmiiboTouchLayout>},
    {"AnimCameraStart", al::createEventFlowNode<al::EventFlowNodeAnimCameraStart>},
    {"AppearMapAmiiboHint", al::createEventFlowNode<EventFlowNodeAppearMapAmiiboHint>},
    {"BgmCtrl", al::createEventFlowNode<EventFlowNodeBgmCtrl>},
    {"BindKeepDemoStart", al::createEventFlowNode<EventFlowNodeBindKeepDemoStart>},
    {"CameraEnd", al::createEventFlowNode<al::EventFlowNodeCameraEnd>},
    {"CameraStart", al::createEventFlowNode<al::EventFlowNodeCameraStart>},
    {"CapMessage", al::createEventFlowNode<EventFlowNodeCapMessage>},
    {"CapManHeroTalkSetDemoStartPose",
     al::createEventFlowNode<EventFlowNodeCapManHeroTalkSetDemoStartPose>},
    {"CapManHeroTalkAppear", al::createEventFlowNode<EventFlowNodeCapManHeroTalkAppear>},
    {"CapManHeroTalkFocus", al::createEventFlowNode<EventFlowNodeCapManHeroTalkFocus>},
    {"CapManHeroTalkPlayerTurn", al::createEventFlowNode<EventFlowNodeCapManHeroTalkPlayerTurn>},
    {"CapManHeroTalkReturn", al::createEventFlowNode<EventFlowNodeCapManHeroTalkReturn>},
    {"CapManHeroTalkSetDemoEndPose",
     al::createEventFlowNode<EventFlowNodeCapManHeroTalkSetDemoEndPose>},
    {"ChangeStage", al::createEventFlowNode<EventFlowNodeChangeStage>},
    {"ChangeWorldDemoMessage", al::createEventFlowNode<EventFlowNodeChangeWorldDemoMessage>},
    {"CheckClear3CollectBgm", al::createEventFlowNode<EventFlowNodeCheckClear3CollectBgm>},
    {"CheckCompleteCollectBgm", al::createEventFlowNode<EventFlowNodeCheckCompleteCollectBgm>},
    {"CheckCostume", al::createEventFlowNode<EventFlowNodeCheckCostume>},
    {"CheckCostumeInvisible", al::createEventFlowNode<EventFlowNodeCheckCostumeInvisible>},
    {"CheckCostumeMissMatchPart", al::createEventFlowNode<EventFlowNodeCheckCostumeMissMatchPart>},
    {"CheckCostumePair", al::createEventFlowNode<EventFlowNodeCheckCostumePair>},
    {"CheckCount", al::createEventFlowNode<EventFlowNodeCheckCount>},
    {"CheckFirstTalkCollectBgm", al::createEventFlowNode<EventFlowNodeCheckFirstTalkCollectBgm>},
    {"CheckFlag", al::createEventFlowNode<EventFlowNodeCheckFlag>},
    {"CheckGetLinkShine", al::createEventFlowNode<EventFlowNodeCheckGetLinkShine>},
    {"CheckLifeUpItem", al::createEventFlowNode<EventFlowNodeCheckLifeUpItem>},
    {"CheckLink", al::createEventFlowNode<EventFlowNodeCheckLink>},
    {"CheckMoonLockOpened", al::createEventFlowNode<EventFlowNodeCheckMoonLockOpened>},
    {"CheckOpenDoorSnow", al::createEventFlowNode<EventFlowNodeCheckOpenDoorSnow>},
    {"CheckPlayingCollectBgm", al::createEventFlowNode<EventFlowNodeCheckPlayingCollectBgm>},
    {"CheckPlayerOnGround", al::createEventFlowNode<EventFlowNodeCheckPlayerOnGround>},
    {"CheckYukimaruRaceResult", al::createEventFlowNode<EventFlowNodeCheckYukimaruRaceResult>},
    {"CheckSwitch", al::createEventFlowNode<al::EventFlowNodeCheckSwitch>},
    {"CheckWaitSwitch", al::createEventFlowNode<al::EventFlowNodeCheckWaitSwitch>},
    {"CloseTalkMessage", al::createEventFlowNode<EventFlowNodeCloseTalkMessage>},
    {"CoinPayment", al::createEventFlowNode<EventFlowNodeCoinPayment>},
    {"CutSceneDemoStart", al::createEventFlowNode<EventFlowNodeCutSceneDemoStart>},
    {"DemoAction", al::createEventFlowNode<al::EventFlowNodeDemoAction>},
    {"DemoCamera", al::createEventFlowNode<al::EventFlowNodeDemoCamera>},
    {"DemoEnd", al::createEventFlowNode<EventFlowNodeDemoEnd>},
    {"DemoForceStartOnGround", al::createEventFlowNode<EventFlowNodeDemoForceStartOnGround>},
    {"DemoForceStart", al::createEventFlowNode<EventFlowNodeDemoForceStart>},
    {"DemoPlayerAction", al::createEventFlowNode<al::EventFlowNodeDemoPlayerAction>},
    {"DemoPlayerHide", al::createEventFlowNode<al::EventFlowNodeDemoPlayerHide>},
    {"DemoPlayerShow", al::createEventFlowNode<al::EventFlowNodeDemoPlayerShow>},
    {"DemoResetPlayerDynamics", al::createEventFlowNode<EventFlowNodeDemoResetPlayerDynamics>},
    {"DemoStart", al::createEventFlowNode<EventFlowNodeDemoStart>},
    {"DirectGetLinkShine", al::createEventFlowNode<EventFlowNodeDirectGetLinkShine>},
    {"EnableHint", al::createEventFlowNode<EventFlowNodeEnableHint>},
    {"Event", al::createEventFlowNode<al::EventFlowNodeEvent>},
    {"EventQuery", al::createEventFlowNode<al::EventFlowNodeEventQuery>},
    {"FirstTalkEndCollectBgmNpc", al::createEventFlowNode<EventFlowNodeFirstTalkEndCollectBgmNpc>},
    {"Fork", al::createEventFlowNode<al::EventFlowNodeFork>},
    {"ForcePutOnDemoCap", al::createEventFlowNode<EventFlowNodeForcePutOnDemoCap>},
    {"GetAmiiboCostume", al::createEventFlowNode<EventFlowNodeGetAmiiboCostume>},
    {"GetAmiiboNotSearchHintNum", al::createEventFlowNode<EventFlowNodeGetAmiiboNotSearchHintNum>},
    {"GetCollectBgmBonus01", al::createEventFlowNode<EventFlowNodeGetCollectBgmBonus01>},
    {"GetCollectBgmBonus02", al::createEventFlowNode<EventFlowNodeGetCollectBgmBonus02>},
    {"GetSearchAmiibo", al::createEventFlowNode<EventFlowNodeGetSearchAmiibo>},
    {"HitReaction", al::createEventFlowNode<al::EventFlowNodeHitReaction>},
    {"IsCostumeAmiibo", al::createEventFlowNode<EventFlowNodeIsCostumeAmiibo>},
    {"IsEnableSearchAmiibo", al::createEventFlowNode<EventFlowNodeIsEnableSearchAmiibo>},
    {"IsTalkAmiiboHelp", al::createEventFlowNode<EventFlowNodeIsTalkAmiiboHelp>},
    {"KakkuTurn", al::createEventFlowNode<EventFlowNodeKakkuTurn>},
    {"Join", al::createEventFlowNode<al::EventFlowNodeJoin>},
    {"JumpEntry", al::createEventFlowNode<al::EventFlowNodeJumpEntry>},
    {"MessageBalloon", al::createEventFlowNode<EventFlowNodeMessageBalloon>},
    {"MessageTalk", al::createEventFlowNode<EventFlowNodeMessageTalk>},
    {"MessageTalkSpecialPurpose", al::createEventFlowNode<EventFlowNodeMessageTalkSpecialPurpose>},
    {"NextTalkMessage", al::createEventFlowNode<EventFlowNodeNextTalkMessage>},
    {"NormalDemoTryStart", al::createEventFlowNode<EventFlowNodeNormalDemoTryStart>},
    {"NpcMoveToLink", al::createEventFlowNode<EventFlowNodeNpcMoveToLink>},
    {"OpenBgmList", al::createEventFlowNode<EventFlowNodeOpenBgmList>},
    {"PlayerAction", al::createEventFlowNode<EventFlowNodePlayerAction>},
    {"PlayerTurn", al::createEventFlowNode<EventFlowNodePlayerTurn>},
    {"PopItem", al::createEventFlowNode<EventFlowNodePopItem>},
    {"PopLinkShine", al::createEventFlowNode<EventFlowNodePopLinkShine>},
    {"QueryJudge", al::createEventFlowNode<al::EventFlowNodeQueryJudge>},
    {"ReplacePlayer", al::createEventFlowNode<EventFlowNodeReplacePlayer>},
    {"SceneWipeClose", al::createEventFlowNode<EventFlowNodeSceneWipeClose>},
    {"SelectChoice", al::createEventFlowNode<EventFlowNodeSelectChoice>},
    {"SelectYesNo", al::createEventFlowNode<EventFlowNodeSelectYesNo>},
    {"SessionWaitMusician", al::createEventFlowNode<EventFlowNodeSessionWaitMusician>},
    {"SetDemoInfoDemoName", al::createEventFlowNode<EventFlowNodeSetDemoInfoDemoName>},
    {"SwitchOn", al::createEventFlowNode<al::EventFlowNodeSwitchOn>},
    {"SwitchOff", al::createEventFlowNode<al::EventFlowNodeSwitchOff>},
    {"TurnToPlayer", al::createEventFlowNode<al::EventFlowNodeTurnToPlayer>},
    {"TurnToPlayerActionOneTime",
     al::createEventFlowNode<al::EventFlowNodeTurnToPlayerActionOneTime>},
    {"TurnToPreDir", al::createEventFlowNode<al::EventFlowNodeTurnToPreDir>},
    {"UnlockHint", al::createEventFlowNode<EventFlowNodeUnlockHint>},
    {"WaitWipeOpenEnd", al::createEventFlowNode<EventFlowNodeWaitWipeOpenEnd>},
    {"WaitSimple", al::createEventFlowNode<EventFlowNodeWaitSimple>},
    {"WipeFadeBlackClose", al::createEventFlowNode<EventFlowNodeWipeFadeBlackClose>},
    {"WipeFadeBlackOpen", al::createEventFlowNode<EventFlowNodeWipeFadeBlackOpen>},
};

ProjectEventFlowNodeFactory::ProjectEventFlowNodeFactory() : EventFlowNodeFactory() {
    initFactory(sProjectEventFlowNodeFactoryEntries);
}
