#include "Scene/ProjectActorFactory.h"

#include "Library/Fluid/FlowMapParts.h"
#include "Library/LiveActor/CreateActorFunction.h"
#include "Library/MapObj/BackHideParts.h"
#include "Library/MapObj/ClockMapParts.h"
#include "Library/MapObj/ConveyerMapParts.h"
#include "Library/MapObj/FallMapParts.h"
#include "Library/MapObj/FixMapParts.h"
#include "Library/MapObj/FloaterMapParts.h"
#include "Library/MapObj/GateMapParts.h"
#include "Library/MapObj/KeyMoveMapParts.h"
#include "Library/MapObj/KeyMoveMapPartsGenerator.h"
#include "Library/MapObj/OneMeshFixMapParts.h"
#include "Library/MapObj/RailMoveMapParts.h"
#include "Library/MapObj/RollingCubeMapParts.h"
#include "Library/MapObj/RotateMapParts.h"
#include "Library/MapObj/SeesawMapParts.h"
#include "Library/MapObj/SlideMapParts.h"
#include "Library/MapObj/SubActorLodMapParts.h"
#include "Library/MapObj/SurfMapParts.h"
#include "Library/MapObj/SwingMapParts.h"
#include "Library/MapObj/SwitchDitherMapParts.h"
#include "Library/MapObj/SwitchOpenMapParts.h"
#include "Library/MapObj/VisibleSwitchMapParts.h"
#include "Library/MapObj/WheelMapParts.h"
#include "Library/MapObj/WobbleMapParts.h"
#include "Library/Obj/AllDeadWatcher.h"
#include "Library/Obj/BgmPlayObj.h"
#include "Library/Obj/CameraRailHolder.h"
#include "Library/Obj/CameraWatchPoint.h"
#include "Library/Obj/EffectObj.h"
#include "Library/Obj/EffectObjCameraEmit.h"
#include "Library/Obj/EffectObjFollowCamera.h"
#include "Library/Obj/EffectObjFollowCameraLimit.h"
#include "Library/Obj/EffectObjInterval.h"

#include "Boss/BarrierField.h"
#include "Boss/BossForest/BossForestWander.h"
#include "Boss/Mofumofu/MofumofuScrap.h"
#include "Camera/ScenarioStartCamera.h"
#include "Demo/DemoPeachWedding.h"
#include "Enemy/Bubble.h"
#include "Enemy/CatchBomb.h"
#include "Enemy/DonkeyKong2D.h"
#include "Enemy/Gamane.h"
#include "Enemy/KaronWing.h"
#include "Enemy/Kuribo2D.h"
#include "Enemy/KuriboMini.h"
#include "Enemy/Megane.h"
#include "Enemy/Mummy.h"
#include "Enemy/Nokonoko2D.h"
#include "Enemy/Pecho.h"
#include "Enemy/Togezo.h"
#include "Enemy/Togezo2D.h"
#include "Item/Coin.h"
#include "Item/Coin2D.h"
#include "Item/Coin2DCityDirector.h"
#include "Item/CoinBlow.h"
#include "Item/CoinChameleon.h"
#include "Item/CoinCirclePlacement.h"
#include "Item/CoinCollect.h"
#include "Item/CoinCollect2D.h"
#include "Item/CoinLead.h"
#include "Item/CoinRail.h"
#include "Item/CoinStackGroup.h"
#include "Item/LifeMaxUpItem.h"
#include "Item/LifeMaxUpItem2D.h"
#include "Item/LifeUpItem.h"
#include "Item/LifeUpItem2D.h"
#include "Item/Shine.h"
#include "MapObj/AirBubble.h"
#include "MapObj/AllDeadWatcherWithShine.h"
#include "MapObj/AnagramAlphabet.h"
#include "MapObj/Barrel2D.h"
#include "MapObj/BlockBrickBig2D.h"
#include "MapObj/BlockEmpty2D.h"
#include "MapObj/BlockQuestion2D.h"
#include "MapObj/BossKnuckleFix.h"
#include "MapObj/BreakablePole.h"
#include "MapObj/CameraSub.h"
#include "MapObj/CapBomb.h"
#include "MapObj/CapHanger.h"
#include "MapObj/CapSwitch.h"
#include "MapObj/Car.h"
#include "MapObj/CarWatcher.h"
#include "MapObj/CheckpointFlag.h"
#include "MapObj/ChurchDoor.h"
#include "MapObj/CitySignal.h"
#include "MapObj/CoinCollectHintObj.h"
#include "MapObj/CollectBgmSpeaker.h"
#include "MapObj/CollectionList.h"
#include "MapObj/DelaySwitch.h"
#include "MapObj/DoorCity.h"
#include "MapObj/DoorSnow.h"
#include "MapObj/Doshi.h"
#include "MapObj/EffectObjAlpha.h"
#include "MapObj/ElectricWire/ElectricWire.h"
#include "MapObj/FireDrum2D.h"
#include "MapObj/FixMapPartsAppearKillAsync.h"
#include "MapObj/FixMapPartsBgmChangeAction.h"
#include "MapObj/HackFork.h"
#include "MapObj/HipDropMoveLift.h"
#include "MapObj/HipDropRepairParts.h"
#include "MapObj/HipDropSwitch.h"
#include "MapObj/HomeInside.h"
#include "MapObj/HomeShip.h"
#include "MapObj/KoopaShip.h"
#include "MapObj/LavaFryingPan.h"
#include "MapObj/LavaPan.h"
#include "MapObj/LavaWave.h"
#include "MapObj/MeganeMapParts.h"
#include "MapObj/MoonBasementBreakParts.h"
#include "MapObj/MoonBasementFloor.h"
#include "MapObj/MoonBasementSlideObj.h"
#include "MapObj/MoonWorldCaptureParadeLift.h"
#include "MapObj/MoviePlayerMapParts.h"
#include "MapObj/PeachWorldTree.h"
#include "MapObj/PlayerMotionObserver.h"
#include "MapObj/PoleClimbParts.h"
#include "MapObj/PoleGrabCeil.h"
#include "MapObj/QuestObj.h"
#include "MapObj/ReactionMapParts.h"
#include "MapObj/RiseMapParts.h"
#include "MapObj/RiseMapPartsHolder.h"
#include "MapObj/RocketFlower.h"
#include "MapObj/RouletteSwitch.h"
#include "MapObj/SaveFlagCheckObj.h"
#include "MapObj/ShineTowerRocket.h"
#include "MapObj/ShopMark.h"
#include "MapObj/ShoppingWatcher.h"
#include "MapObj/SignBoard.h"
#include "MapObj/SignBoardDanger.h"
#include "MapObj/SnowVolumeEraser.h"
#include "MapObj/Souvenir.h"
#include "MapObj/Special2KeyMoveLift.h"
#include "MapObj/StageSwitchSelector.h"
#include "MapObj/StatueSnapMark.h"
#include "MapObj/TalkPoint.h"
#include "MapObj/TrampleBush.h"
#include "MapObj/TrampleSwitch.h"
#include "MapObj/TrampleSwitchTimer.h"
#include "MapObj/TransparentWall.h"
#include "MapObj/TreasureBoxKey.h"
#include "MapObj/VolleyballBase.h"
#include "MapObj/VolleyballNet.h"
#include "MapObj/WaterfallWorldWaterfall.h"
#include "MapObj/WeightSwitch.h"
#include "MapObj/WorldMapEarth.h"
#include "MapObj/WorldWarpHole.h"
#include "Npc/Bird.h"
#include "Npc/BirdPlayerGlideCtrl.h"
#include "Npc/FlyObject.h"
#include "Npc/KuriboGirl.h"
#include "Npc/RaceAudienceNpc.h"
#include "Npc/SessionMusicianBgmCtrlObj.h"
#include "Npc/SessionMusicianNpc.h"
#include "Npc/VocalMike.h"
#include "Npc/VolleyballNpc.h"
#include "Npc/WorldTravelingNpc.h"
#include "Player/Yoshi.h"

namespace al {
template <>
LiveActor* createActorFunction<void>(const char*) {
    return nullptr;
}

using AtmosScatterRequester = void;
using EntranceCameraStartObj = void;
using FogRequester = void;
using GraphicsObjShadowMaskSphere = void;
using KeyMoveCameraObj = void;
using LightningController = void;
using OccludedEffectRequester = void;
using PrePassLineLight = void;
using PrePassPointLight = void;
using PrePassProjLight = void;
using PrePassProjOrthoLight = void;
using PrePassSpotLight = void;
using RippleFixMapParts = void;
using RippleGeneratePoint = void;
using SeBarrierObj = void;
using SePlayObj = void;
using SePlayRail = void;
using Sky = void;
using SwitchKeepOnWatcher = void;
using SwitchKeyMoveMapParts = void;
using ThunderRenderRequester = void;
using WaterAreaMoveModel = void;
}  // namespace al

using AchievementNpc = void;
using AirBubbleGenerator = void;
using AirCurrent = void;
using AmiiboHelpNpc = void;
using AmiiboNpc = void;
using BarrelGenerator2D = void;
using BazookaElectric = void;
using BendLeafTree = void;
using BirdCarryMeat = void;
using BlockBrick = void;
using BlockBrick2D = void;
using BlockEmpty = void;
using BlockHard = void;
using BlockQuestion = void;
using BlockTransparent = void;
using BlockTransparent2D = void;
using BlowObj = void;
using BombTail = void;
using BossForest = void;
using BossForestBlock = void;
using BossKnuckle = void;
using BossKnuckleCounterGround = void;
using BossMagma = void;
using BossRaid = void;
using BossRaidNpc = void;
using BossRaidRivet = void;
using BreakMapParts = void;
using Breeda = void;
using Bubble2D = void;
using BubbleLauncher = void;
using Bull = void;
using Byugo = void;
using Cactus = void;
using CactusMini = void;
using CageShineWatcher = void;
using CageSwitch = void;
using CameraDemoGateMapParts = void;
using CameraDemoKeyMoveMapParts = void;
using Candlestand = void;
using CandlestandBgmDirector = void;
using CandlestandInitializer = void;
using CandlestandSaveWatcher = void;
using CandlestandWatcher = void;
using CapAccelerator = void;
using CapAppearMapParts = void;
using CapBeamer = void;
using CapCatapult = void;
using CapFlower = void;
using CapFlowerGroup = void;
using CapMessageAfterInformation = void;
using CapMessagePlacement = void;
using CapRack = void;
using CapRailMover = void;
using CapRotateMapParts = void;
using CapSlotBase = void;
using CapSwitchTimer = void;
using CapThrower = void;
using CapTrampoline = void;
using CardboardBox = void;
using Chair = void;
using ChorobonHolder = void;
using CityBuilding = void;
using CitySign = void;
using CityStreetlight = void;
using CityWorldSign = void;
using CityWorldUndergroundMachine = void;
using Closet = void;
using CloudStep = void;
using CoinRing = void;
using CollapseSandHill = void;
using CollectAnimalWatcher = void;
using CrystalBreakable = void;
using DamageBallGenerator = void;
using DemoActorCapManHero = void;
using DemoActorCapManHeroine = void;
using DemoActorHack = void;
using DemoActorKoopaShip = void;
using DemoActorPeach = void;
using DemoActorShineTower = void;
using DemoChangeEffectObj = void;
using DemoPlayer = void;
using DemoPlayerCap = void;
using DemoWorldMoveHomeBackGround = void;
using DigPoint = void;
using Dokan = void;
using DokanKoopa = void;
using DokanMaze = void;
using DokanMazeDirector = void;
using DokanStageChange = void;
using Donsuke = void;
using DoorAreaChange = void;
using DoorAreaChangeCap = void;
using DoorWarp = void;
using DoorWarpStageChange = void;
using EchoBlockMapParts = void;
using EffectObjNpcManFar = void;
using EffectObjQualityChange = void;
using ElectricWireKoopa = void;
using EventKeyMoveCameraObjNoDemo = void;
using EventKeyMoveCameraObjWithDemo = void;
using Fastener = void;
using FastenerObj = void;
using FigureWalkingNpc = void;
using FireBlower = void;
using FireBros = void;
using FireHydrant = void;
using FireSwitch = void;
using FishingFish = void;
using FixMapParts2D = void;
using FixMapPartsCapHanger = void;
using FixMapPartsDitherAppear = void;
using FixMapPartsForceSafetyPoint = void;
using FixMapPartsFukankunZoomCapMessage = void;
using FixMapPartsScenarioAction = void;
using ForestManSeed = void;
using ForestWorldEnergyStand = void;
using ForestWorldFlowerCtrl = void;
using FrailBox = void;
using Frog = void;
using Fukankun = void;
using FukankunZoomCapMessageSun = void;
using FukuwaraiWatcher = void;
using GabuZou = void;
using GabuZouGroup = void;
using GiantWanderBoss = void;
using GoalMark = void;
using GolemClimb = void;
using Gotogoton = void;
using GotogotonGoal = void;
using GrowerBug = void;
using GrowerWorm = void;
using GrowFlowerCoin = void;
using GrowFlowerWatcher = void;
using GrowPlantGrowPlace = void;
using GrowPlantSeed = void;
using GrowPlantStartStage = void;
using GrowPlantWatcher = void;
using Gunetter = void;
using GunetterSpin = void;
using HackCar = void;
using HackMapParts = void;
using HammerBros = void;
using HammerBros2D = void;
using HelpNpc = void;
using HintNpc = void;
using HintPhoto = void;
using HintRouteGuidePoint = void;
using HipDropSwitchTimer = void;
using HipDropTile = void;
using HipDropTransformPartsWatcher = void;
using HomeBed = void;
using HomeChair = void;
using Hosui = void;
using IcicleFall = void;
using Imomu = void;
using IndicatorDirector = void;
using Jango = void;
using Joku = void;
using JugemFishing = void;
using JumpingRopeNpc = void;
using Kakku = void;
using KickStone = void;
using KillerLauncher = void;
using KillerLauncherDot = void;
using KinokoUfo = void;
using Koopa = void;
using KoopaCapPlayer = void;
using KoopaChurch = void;
using KoopaLv1 = void;
using KuriboGenerator2D = void;
using KuriboHack = void;
using KuriboTowerSwitch = void;
using KuriboWing = void;
using LavaStewVeget = void;
using LongGenerator = void;
using MapPartsRoulette = void;
using MarchingCubeBlock = void;
using MeganeKeyMoveMapParts = void;
using MeganeLiftExLift = void;
using Mirror = void;
using Mofumofu = void;
using MoonBasementClimaxWatcher = void;
using MoonBasementFallObjDecoration = void;
using MoonBasementFinalGate = void;
using MoonBasementGate = void;
using MoonBasementMeteorAreaObj = void;
using MoonBasementMeteorPointObj = void;
using MoonBasementPillar = void;
using MoonBasementRock = void;
using MoonRock = void;
using MoonWorldBell = void;
using Motorcycle = void;
using MotorcycleParkingLot = void;
using MoveHomeNpc = void;
using MultiGateKeeperBonfire = void;
using MultiGateKeeperWatcher = void;
using MummyGenerator = void;
using NeedleTrap = void;
using NoteObj = void;
using NoteObj2D = void;
using NoteObjDirector = void;
using Objex = void;
using OceanWaveActor = void;
using OpeningStageStartDemo = void;
using PackunFire = void;
using PadRumblePoint = void;
using PaintObj = void;
using PaulineAtCeremony = void;
using PaulineAudience = void;
using PeachCastleCap = void;
using PeachWorldGate = void;
using PeachWorldMoatWater = void;
using Pen = void;
using PictureStageChange = void;
using PillarKeyMoveParts = void;
using PillarSwitchOpenMapParts = void;
using PlayerStartObj = void;
using PlayerSubjectiveWatchCheckObj = void;
using PlayGuideBoard = void;
using PlayRecorder = void;
using PochiHintPhoto = void;
using Poetter = void;
using PopnGenerator = void;
using PosterWatcher = void;
using PrePassCausticsLight = void;
using ProjectGraphicsObjShadowMaskCube = void;
using ProjectRaceCheckPoint = void;
using Pukupuku = void;
using PulseSwitch = void;
using Pyramid = void;
using RabbitGraph = void;
using RaceManGoal = void;
using RaceManRace = void;
using RaceManStart = void;
using RaceWatcher = void;
using RadiconNpc = void;
using RadiConRaceWatcher = void;
using RadioCassette = void;
using Radish = void;
using RailCollision = void;
using RailDrawer = void;
using RankingNpc = void;
using ReactionObject = void;
using ReactionObjectCarBreakable = void;
using ReactionObjectPoster = void;
using ReactionObjectSkyRhythm = void;
using ReflectBombGenerator = void;
using RhythmSpotlight = void;
using RotateTarget = void;
using RouteGuideArrow = void;
using RouteGuideRail = void;
using RunAwayNpc = void;
using SandGeyser = void;
using SandWorldHomeLift = void;
using SaucePan = void;
using Senobi = void;
using SenobiGeneratePoint = void;
using SenobiMoveMapParts = void;
using SenobiMoveMapPartsConnector = void;
using SePlayObjWithSave = void;
using SequentialSwitch = void;
using SessionMayorNpc = void;
using Shibaken = void;
using ShineChipWatcher = void;
using ShineFukankunWatchObj = void;
using ShopBgmPlayer = void;
using SignBoardLayoutTexture = void;
using SkyFukankunZoomCapMessage = void;
using SkyWorldCloud = void;
using SkyWorldKoopaFire = void;
using SkyWorldKoopaFrame = void;
using SkyWorldMiddleViewCloud = void;
using SmallWanderBoss = void;
using SneakingMan = void;
using SnowManRaceNpc = void;
using SnowVolume = void;
using SnowWorldBigIcicle = void;
using SnowWorldSequenceFlagCheckObj = void;
using SouvenirDirector = void;
using SphinxQuiz = void;
using SphinxRide = void;
using SphinxTaxiWatcher = void;
using Squirrel = void;
using Stacker = void;
using StackerCapWorldCtrl = void;
using StageEventDemo = void;
using StageTalkDemoNpcCap = void;
using Stake = void;
using Statue = void;
using SubActorLodFixPartsScenarioAction = void;
using SwitchAnd = void;
using TalkMessageInfoPointSaveObj = void;
using TalkNpc = void;
using TalkNpcFreeze = void;
using Tank = void;
using TankReviveCtrl = void;
using TaxiStop = void;
using TextureReplaceScreen = void;
using TokimekiMayorNpc = void;
using TreasureBox = void;
using TreasureBoxSequentialDirector = void;
using TRex = void;
using TRexScrollBreakMapParts = void;
using Tsukkun = void;
using TsukkunHole = void;
using TwistChainList = void;
using Utsubo = void;
using UtsuboWatcher = void;
using Wanwan = void;
using WanwanHole = void;
using WaterfallWorldBigBreakableWall = void;
using WaterfallWorldFallDownBridge = void;
using WaterRoad = void;
using WaveSurfMapParts = void;
using WheelWaveSurfParts = void;
using WindBlowMapParts = void;
using WindBlowPuzzle = void;
using WorldTravelingPeach = void;
using YoshiFruit = void;
using YoshiFruitShineHolder = void;
using Yukimaru = void;
using YukimaruRacer = void;
using YukimaruRacerTiago = void;

const al::NameToCreator<al::ActorCreatorFunction> sProjectActorFactoryEntries[] = {
    {"AchievementNpc", al::createActorFunction<AchievementNpc>},
    {"AirBubble", al::createActorFunction<AirBubble>},
    {"AirBubbleGenerator", al::createActorFunction<AirBubbleGenerator>},
    {"AirCurrent", al::createActorFunction<AirCurrent>},
    {"AllDeadWatcher", al::createActorFunction<al::AllDeadWatcher>},
    {"AllDeadWatcherWithShine", al::createActorFunction<AllDeadWatcherWithShine>},
    {"AmiiboHelpNpc", al::createActorFunction<AmiiboHelpNpc>},
    {"AmiiboNpc", al::createActorFunction<AmiiboNpc>},
    {"AnagramAlphabet", al::createActorFunction<AnagramAlphabet>},
#ifdef VER_120
    {"TimeBalloonNpc", al::createActorFunction<TimeBalloonNpc>},
#endif
    {"Barrel2D", al::createActorFunction<Barrel2D>},
    {"BarrelGenerator2D", al::createActorFunction<BarrelGenerator2D>},
    {"BarrierField", al::createActorFunction<BarrierField>},
    {"BazookaElectric", al::createActorFunction<BazookaElectric>},
    {"BendLeafTree", al::createActorFunction<BendLeafTree>},
    {"BgmPlayObj", al::createActorFunction<al::BgmPlayObj>},
    {"Bird", al::createActorFunction<Bird>},
    {"BirdCarryMeat", al::createActorFunction<BirdCarryMeat>},
    {"BirdPlayerGlideCtrl", al::createActorFunction<BirdPlayerGlideCtrl>},
    {"BlockBrick", al::createActorFunction<BlockBrick>},
    {"BlockBrick2D", al::createActorFunction<BlockBrick2D>},
    {"BlockBrickBig2D", al::createActorFunction<BlockBrickBig2D>},
    {"BlockEmpty", al::createActorFunction<BlockEmpty>},
    {"BlockEmpty2D", al::createActorFunction<BlockEmpty2D>},
    {"BlockHard", al::createActorFunction<BlockHard>},
    {"ClashWorldBlockHard", al::createActorFunction<BlockHard>},
    {"BlockQuestion", al::createActorFunction<BlockQuestion>},
    {"CityBlockQuestion", al::createActorFunction<BlockQuestion>},
    {"BlockQuestion2D", al::createActorFunction<BlockQuestion2D>},
    {"BlockTransparent", al::createActorFunction<BlockTransparent>},
    {"BlockTransparent2D", al::createActorFunction<BlockTransparent2D>},
    {"BlowObjBeans", al::createActorFunction<BlowObj>},
    {"BlowObjCan", al::createActorFunction<BlowObj>},
    {"BlowObjGarbageBag", al::createActorFunction<BlowObj>},
    {"BlowObjMushroom", al::createActorFunction<BlowObj>},
    {"BlowObj", al::createActorFunction<BlowObj>},
    {"BombTail", al::createActorFunction<BombTail>},
    {"BossForest", al::createActorFunction<BossForest>},
    {"BossForestBlock", al::createActorFunction<BossForestBlock>},
    {"BossForestWander", al::createActorFunction<BossForestWander>},
    {"BossKnuckle", al::createActorFunction<BossKnuckle>},
    {"BossKnuckleCounterGround", al::createActorFunction<BossKnuckleCounterGround>},
    {"BossKnuckleFix", al::createActorFunction<BossKnuckleFix>},
    {"BossMagma", al::createActorFunction<BossMagma>},
    {"BossRaid", al::createActorFunction<BossRaid>},
    {"BossRaidNpc", al::createActorFunction<BossRaidNpc>},
    {"BossRaidRivet", al::createActorFunction<BossRaidRivet>},
    {"BreakablePole", al::createActorFunction<BreakablePole>},
    {"Breeda", al::createActorFunction<Breeda>},
    {"Bubble", al::createActorFunction<Bubble>},
    {"Bubble2D", al::createActorFunction<Bubble2D>},
    {"BubbleLauncher", al::createActorFunction<BubbleLauncher>},
    {"Bull", al::createActorFunction<Bull>},
    {"Byugo", al::createActorFunction<Byugo>},
    {"Cactus", al::createActorFunction<Cactus>},
    {"CactusMini", al::createActorFunction<CactusMini>},
    {"CageShine", al::createActorFunction<CageShineWatcher>},
    {"CageSaveSwitch", al::createActorFunction<CageSwitch>},
    {"CageStageSwitch", al::createActorFunction<CageSwitch>},
    {"CageBreakable", al::createActorFunction<BreakMapParts>},
    {"CameraDemoGateMapParts", al::createActorFunction<CameraDemoGateMapParts>},
    {"CameraDemoKeyMoveMapParts", al::createActorFunction<CameraDemoKeyMoveMapParts>},
    {"CameraRailHolder", al::createActorFunction<al::CameraRailHolder>},
    {"CameraSub", al::createActorFunction<CameraSub>},
    {"CameraWatchPoint", al::createActorFunction<al::CameraWatchPoint>},
    {"Candlestand", al::createActorFunction<Candlestand>},
    {"CandlestandFire", al::createActorFunction<Candlestand>},
    {"CandlestandInitializer", al::createActorFunction<CandlestandInitializer>},
    {"CandlestandBgmDirector", al::createActorFunction<CandlestandBgmDirector>},
    {"CandlestandSaveWatcher", al::createActorFunction<CandlestandSaveWatcher>},
    {"CandlestandWatcher", al::createActorFunction<CandlestandWatcher>},
    {"CapAccelerator", al::createActorFunction<CapAccelerator>},
    {"CapAcceleratorKeyMoveMapParts", al::createActorFunction<CapAccelerator>},
    {"CapAppearMapParts", al::createActorFunction<CapAppearMapParts>},
    {"CapBeamer", al::createActorFunction<CapBeamer>},
    {"CapBomb", al::createActorFunction<CapBomb>},
    {"CapCatapult", al::createActorFunction<CapCatapult>},
    {"CapFlower", al::createActorFunction<CapFlower>},
    {"CapFlowerGroup", al::createActorFunction<CapFlowerGroup>},
    {"CapHanger", al::createActorFunction<CapHanger>},
    {"CapMessageAfterInformation", al::createActorFunction<CapMessageAfterInformation>},
    {"CapRack", al::createActorFunction<CapRack>},
    {"CapRackTimer", al::createActorFunction<CapRack>},
    {"CapRailMover", al::createActorFunction<CapRailMover>},
    {"CapSlotBase", al::createActorFunction<CapSlotBase>},
    {"CapSwitch", al::createActorFunction<CapSwitch>},
    {"CapSwitchSave", al::createActorFunction<CapSwitch>},
    {"CapSwitchTimer", al::createActorFunction<CapSwitchTimer>},
    {"CapThrower", al::createActorFunction<CapThrower>},
    {"CapTrampoline", al::createActorFunction<CapTrampoline>},
    {"Car", al::createActorFunction<Car>},
    {"CarSandWorld", al::createActorFunction<Car>},
    {"CarWatcher", al::createActorFunction<CarWatcher>},
    {"CardboardBox", al::createActorFunction<CardboardBox>},
    {"CatchBomb", al::createActorFunction<CatchBomb>},
    {"Chair", al::createActorFunction<Chair>},
    {"CheckpointFlag", al::createActorFunction<CheckpointFlag>},
    {"ChorobonHolder", al::createActorFunction<ChorobonHolder>},
    {"ChurchDoor", al::createActorFunction<ChurchDoor>},
    {"CityBuilding", al::createActorFunction<CityBuilding>},
    {"CityStreetlight", al::createActorFunction<CityStreetlight>},
    {"CityWorldSign", al::createActorFunction<CityWorldSign>},
    {"CityWorldUndergroundMachine", al::createActorFunction<CityWorldUndergroundMachine>},
    {"CitySign", al::createActorFunction<CitySign>},
    {"CitySignal", al::createActorFunction<CitySignal>},
    {"CityWorldTable", al::createActorFunction<ReactionObject>},
    {"Closet", al::createActorFunction<Closet>},
    {"CloudStep", al::createActorFunction<CloudStep>},
    {"CollapseSandHill", al::createActorFunction<CollapseSandHill>},
    {"CollectAnimalWatcher", al::createActorFunction<CollectAnimalWatcher>},
    {"CollectBgmSpeaker", al::createActorFunction<CollectBgmSpeaker>},
    {"CollectionList", al::createActorFunction<CollectionList>},
    {"Coin", al::createActorFunction<Coin>},
    {"Coin2D", al::createActorFunction<Coin2D>},
    {"Coin2DCityDirector", al::createActorFunction<Coin2DCityDirector>},
    {"CoinBlow", al::createActorFunction<CoinBlow>},
    {"CoinChameleon", al::createActorFunction<CoinChameleon>},
    {"CoinCirclePlacement", al::createActorFunction<CoinCirclePlacement>},
    {"CoinCollect", al::createActorFunction<CoinCollect>},
    {"CoinCollectHintObj", al::createActorFunction<CoinCollectHintObj>},
    {"CoinCollect2D", al::createActorFunction<CoinCollect2D>},
    {"CoinLead", al::createActorFunction<CoinLead>},
    {"CoinRail", al::createActorFunction<CoinRail>},
    {"CoinRing", al::createActorFunction<CoinRing>},
    {"CoinStackGroup", al::createActorFunction<CoinStackGroup>},
    {"CrystalBreakable", al::createActorFunction<CrystalBreakable>},
    {"DamageBallGenerator", al::createActorFunction<DamageBallGenerator>},
    {"DelaySwitch", al::createActorFunction<DelaySwitch>},
    {"DemoActorCapManHero", al::createActorFunction<DemoActorCapManHero>},
    {"DemoActorCapManHeroine", al::createActorFunction<DemoActorCapManHeroine>},
    {"DemoActorKoopaShip", al::createActorFunction<DemoActorKoopaShip>},
    {"DemoActorHack", al::createActorFunction<DemoActorHack>},
    {"DemoActorPeach", al::createActorFunction<DemoActorPeach>},
    {"DemoActorShineTower", al::createActorFunction<DemoActorShineTower>},
    {"DemoPeachWorldHomeWater001", al::createActorFunction<al::RippleFixMapParts>},
    {"DemoChangeEffectObj", al::createActorFunction<DemoChangeEffectObj>},
    {"DemoWorldMoveHomeBackGround", al::createActorFunction<DemoWorldMoveHomeBackGround>},
    {"DemoPeachWedding", al::createActorFunction<DemoPeachWedding>},
    {"DemoPlayer", al::createActorFunction<DemoPlayer>},
    {"DemoPlayerCap", al::createActorFunction<DemoPlayerCap>},
    {"DigPoint", al::createActorFunction<DigPoint>},
    {"DigPointHintPhoto", al::createActorFunction<DigPoint>},
    {"DigPointWater", al::createActorFunction<DigPoint>},
    {"DirectionFixedBillboard", al::createActorFunction<al::FixMapParts>},
    {"Dokan", al::createActorFunction<Dokan>},
    {"DokanKoopa", al::createActorFunction<DokanKoopa>},
    {"DokanMaze", al::createActorFunction<DokanMaze>},
    {"DokanMazeDirector", al::createActorFunction<DokanMazeDirector>},
    {"DokanStageChange", al::createActorFunction<DokanStageChange>},
    {"DonkeyKong2D", al::createActorFunction<DonkeyKong2D>},
    {"Donsuke", al::createActorFunction<Donsuke>},
    {"Doshi", al::createActorFunction<Doshi>},
    {"DoorAreaChange", al::createActorFunction<DoorAreaChange>},
    {"DoorAreaChangeCap", al::createActorFunction<DoorAreaChangeCap>},
    {"DoorCity", al::createActorFunction<DoorCity>},
    {"DoorSnow", al::createActorFunction<DoorSnow>},
    {"DoorWarp", al::createActorFunction<DoorWarp>},
    {"DoorWarpStageChange", al::createActorFunction<DoorWarpStageChange>},
    {"EchoBlockMapParts", al::createActorFunction<EchoBlockMapParts>},
    {"EffectObj", al::createActorFunction<al::EffectObj>},
    {"EffectObjScale", al::createActorFunction<al::EffectObj>},
    {"EffectObjAlpha", al::createActorFunction<EffectObjAlpha>},
    {"EffectObjCameraEmit", al::createActorFunction<al::EffectObjCameraEmit>},
    {"EffectObjFollowCamera", al::createActorFunction<al::EffectObjFollowCamera>},
    {"EffectObjFollowCameraLimit", al::createActorFunction<al::EffectObjFollowCameraLimit>},
    {"EffectObjInterval", al::createActorFunction<al::EffectObjInterval>},
    {"EffectObjNpcManFar", al::createActorFunction<EffectObjNpcManFar>},
    {"EffectObjQualityChange", al::createActorFunction<EffectObjQualityChange>},
    {"ElectricWire", al::createActorFunction<ElectricWire>},
    {"ElectricWireKoopa", al::createActorFunction<ElectricWireKoopa>},
    {"EntranceCameraStartObj", al::createActorFunction<al::EntranceCameraStartObj>},
    {"EventKeyMoveCameraObjNoDemo", al::createActorFunction<EventKeyMoveCameraObjNoDemo>},
    {"EventKeyMoveCameraObjWithDemo", al::createActorFunction<EventKeyMoveCameraObjWithDemo>},
    {"FigureWalkingNpc", al::createActorFunction<FigureWalkingNpc>},
    {"FireBlower", al::createActorFunction<FireBlower>},
    {"FireBrosPossessed", al::createActorFunction<FireBros>},
    {"FireSwitch", al::createActorFunction<FireSwitch>},
    {"FireHydrant", al::createActorFunction<FireHydrant>},
    {"FireDrum2D", al::createActorFunction<FireDrum2D>},
    {"FishingFish", al::createActorFunction<FishingFish>},
    {"FixMapParts2D", al::createActorFunction<FixMapParts2D>},
    {"FixMapPartsAppearKillAsync", al::createActorFunction<FixMapPartsAppearKillAsync>},
    {"FixMapPartsBgmChangeAction", al::createActorFunction<FixMapPartsBgmChangeAction>},
    {"FixMapPartsCapHanger", al::createActorFunction<FixMapPartsCapHanger>},
    {"FixMapPartsDitherAppear", al::createActorFunction<FixMapPartsDitherAppear>},
    {"FixMapPartsForceSafetyPoint", al::createActorFunction<FixMapPartsForceSafetyPoint>},
    {"FixMapPartsFukankunZoomCapMessage",
     al::createActorFunction<FixMapPartsFukankunZoomCapMessage>},
    {"FixMapPartsScenarioAction", al::createActorFunction<FixMapPartsScenarioAction>},
    {"FlyObject", al::createActorFunction<FlyObject>},
    {"ForestManSeed", al::createActorFunction<ForestManSeed>},
    {"ForestWorldHomeBreakParts000", al::createActorFunction<BlockHard>},
    {"FogRequester", al::createActorFunction<al::FogRequester>},
    {"FrailBox", al::createActorFunction<FrailBox>},
    {"Frog", al::createActorFunction<Frog>},
    {"Fukankun", al::createActorFunction<Fukankun>},
    {"FukankunZoomCapMessageSun", al::createActorFunction<FukankunZoomCapMessageSun>},
    {"FukuwaraiWatcher", al::createActorFunction<FukuwaraiWatcher>},
    {"ForestWorldEnergyStand", al::createActorFunction<ForestWorldEnergyStand>},
    {"ForestWorldFlowerCtrl", al::createActorFunction<ForestWorldFlowerCtrl>},
    {"GabuZou", al::createActorFunction<GabuZou>},
    {"GabuZouGroup", al::createActorFunction<GabuZouGroup>},
    {"Gamane", al::createActorFunction<Gamane>},
    {"GiantWanderBoss", al::createActorFunction<GiantWanderBoss>},
    {"GoalMark", al::createActorFunction<GoalMark>},
    {"GolemClimb", al::createActorFunction<GolemClimb>},
    {"Gotogoton", al::createActorFunction<Gotogoton>},
    {"GotogotonGoal", al::createActorFunction<GotogotonGoal>},
    {"GraphicsObjShadowMaskCube", al::createActorFunction<ProjectGraphicsObjShadowMaskCube>},
    {"GraphicsObjShadowMaskSphere", al::createActorFunction<al::GraphicsObjShadowMaskSphere>},
    {"GrowerBug", al::createActorFunction<GrowerBug>},
    {"GrowerWorm", al::createActorFunction<GrowerWorm>},
    {"GrowFlowerCoin", al::createActorFunction<GrowFlowerCoin>},
    {"GrowFlowerWatcher", al::createActorFunction<GrowFlowerWatcher>},
    {"GrowPlantGrowPlace", al::createActorFunction<GrowPlantGrowPlace>},
    {"GrowPlantSeed", al::createActorFunction<GrowPlantSeed>},
    {"GrowPlantStartStage", al::createActorFunction<GrowPlantStartStage>},
    {"GrowPlantWatcher", al::createActorFunction<GrowPlantWatcher>},
    {"Gunetter", al::createActorFunction<GunetterSpin>},
    {"GunetterMove", al::createActorFunction<Gunetter>},
    {"HackCar", al::createActorFunction<HackCar>},
    {"HackFork", al::createActorFunction<HackFork>},
    {"HammerBrosPossessed", al::createActorFunction<HammerBros>},
    {"HammerBros2D", al::createActorFunction<HammerBros2D>},
    {"HelpNpc", al::createActorFunction<HelpNpc>},
    {"HintNpc", al::createActorFunction<HintNpc>},
    {"HintPhoto", al::createActorFunction<HintPhoto>},
    {"HintRouteGuidePoint", al::createActorFunction<HintRouteGuidePoint>},
    {"HipDropSwitch", al::createActorFunction<HipDropSwitch>},
    {"HipDropSwitchSave", al::createActorFunction<HipDropSwitch>},
    {"HipDropSwitchTimer", al::createActorFunction<HipDropSwitchTimer>},
    {"HipDropTile", al::createActorFunction<HipDropTile>},
    {"HipDropMoveLift", al::createActorFunction<HipDropMoveLift>},
    {"HipDropRepairParts", al::createActorFunction<HipDropRepairParts>},
    {"HipDropTransformPartsWatcher", al::createActorFunction<HipDropTransformPartsWatcher>},
    {"HomeBed", al::createActorFunction<HomeBed>},
    {"HomeChair", al::createActorFunction<HomeChair>},
    {"HomeInside", al::createActorFunction<HomeInside>},
    {"HomeShip", al::createActorFunction<HomeShip>},
    {"Hosui", al::createActorFunction<Hosui>},
    {"IcicleFall", al::createActorFunction<IcicleFall>},
    {"Imomu", al::createActorFunction<Imomu>},
    {"IndicatorDirector", al::createActorFunction<IndicatorDirector>},
    {"Jango", al::createActorFunction<Jango>},
    {"Joku", al::createActorFunction<Joku>},
    {"JugemFishing", al::createActorFunction<JugemFishing>},
    {"JumpingRopeNpc", al::createActorFunction<JumpingRopeNpc>},
    {"Kakku", al::createActorFunction<Kakku>},
    {"KaronWing", al::createActorFunction<KaronWing>},
    {"KeyMoveCameraFix", al::createActorFunction<al::KeyMoveCameraObj>},
    {"KickStone", al::createActorFunction<KickStone>},
    {"KillerLauncher", al::createActorFunction<KillerLauncher>},
    {"KillerLauncherDot", al::createActorFunction<KillerLauncherDot>},
    {"KinokoUfo", al::createActorFunction<KinokoUfo>},
    {"Koopa", al::createActorFunction<Koopa>},
    {"KoopaCapPlayer", al::createActorFunction<KoopaCapPlayer>},
    {"KoopaChurch", al::createActorFunction<KoopaChurch>},
    {"KoopaLv1", al::createActorFunction<KoopaLv1>},
    {"KoopaLv2", al::createActorFunction<KoopaLv1>},
    {"KoopaLv3", al::createActorFunction<KoopaLv1>},
    {"KoopaShip", al::createActorFunction<KoopaShip>},
    {"Kuribo2D3D", al::createActorFunction<Kuribo2D>},
    {"KuriboGenerator2D3D", al::createActorFunction<KuriboGenerator2D>},
    {"KuriboGirl", al::createActorFunction<KuriboGirl>},
    {"KuriboPossessed", al::createActorFunction<KuriboHack>},
    {"KuriboMini", al::createActorFunction<KuriboMini>},
    {"KuriboTowerSwitch", al::createActorFunction<KuriboTowerSwitch>},
    {"KuriboWing", al::createActorFunction<KuriboWing>},
    {"LavaFryingPan", al::createActorFunction<LavaFryingPan>},
    {"LavaStewVeget", al::createActorFunction<LavaStewVeget>},
    {"LavaPan", al::createActorFunction<LavaPan>},
    {"LavaWave", al::createActorFunction<LavaWave>},
    {"LifeMaxUpItem", al::createActorFunction<LifeMaxUpItem>},
    {"LifeMaxUpItem2D", al::createActorFunction<LifeMaxUpItem2D>},
    {"LifeUpItem", al::createActorFunction<LifeUpItem>},
    {"LifeUpItem2D", al::createActorFunction<LifeUpItem2D>},
    {"LightningController", al::createActorFunction<al::LightningController>},
    {"LongGenerator", al::createActorFunction<LongGenerator>},
    {"MarchingCubeBlock", al::createActorFunction<MarchingCubeBlock>},
    {"MapPartsRoulette", al::createActorFunction<MapPartsRoulette>},
    {"Megane", al::createActorFunction<Megane>},
    {"MeganeLiftExLift", al::createActorFunction<MeganeLiftExLift>},
    {"MeganeKeyMoveMapParts", al::createActorFunction<MeganeKeyMoveMapParts>},
    {"MeganeMapParts", al::createActorFunction<MeganeMapParts>},
    {"Mirror", al::createActorFunction<Mirror>},
    {"MoonBasementBreakParts", al::createActorFunction<MoonBasementBreakParts>},
    {"MoonBasementClimaxWatcher", al::createActorFunction<MoonBasementClimaxWatcher>},
    {"MoonBasementFallObj", al::createActorFunction<MoonBasementMeteorPointObj>},
    {"MoonBasementFinalGate", al::createActorFunction<MoonBasementFinalGate>},
    {"MoonBasementFallObjDecoration", al::createActorFunction<MoonBasementFallObjDecoration>},
    {"MoonBasementFloor", al::createActorFunction<MoonBasementFloor>},
    {"MoonBasementGate", al::createActorFunction<MoonBasementGate>},
    {"MoonBasementMeteorAreaObj", al::createActorFunction<MoonBasementMeteorAreaObj>},
    {"MoonBasementPillar", al::createActorFunction<MoonBasementPillar>},
    {"MoonBasementRock", al::createActorFunction<MoonBasementRock>},
    {"MoonBasementSlideObj", al::createActorFunction<MoonBasementSlideObj>},
    {"MoonRock", al::createActorFunction<MoonRock>},
    {"MoonWorldBell", al::createActorFunction<MoonWorldBell>},
    {"MoonWorldCaptureParadeLift", al::createActorFunction<MoonWorldCaptureParadeLift>},
    {"Mofumofu", al::createActorFunction<Mofumofu>},
    {"MofumofuLv2", al::createActorFunction<Mofumofu>},
    {"MofumofuScrap", al::createActorFunction<MofumofuScrap>},
    {"Motorcycle", al::createActorFunction<Motorcycle>},
    {"MotorcycleParkingLot", al::createActorFunction<MotorcycleParkingLot>},
    {"MoveHomeNpc", al::createActorFunction<MoveHomeNpc>},
    {"MoviePlayerMapParts", al::createActorFunction<MoviePlayerMapParts>},
    {"MultiGateKeeperBonfire", al::createActorFunction<MultiGateKeeperBonfire>},
    {"MultiGateKeeperWatcher", al::createActorFunction<MultiGateKeeperWatcher>},
    {"Mummy", al::createActorFunction<Mummy>},
    {"MummyGenerator", al::createActorFunction<MummyGenerator>},
    {"NeedleTrap", al::createActorFunction<NeedleTrap>},
    {"Nokonoko2D", al::createActorFunction<Nokonoko2D>},
    {"NoteObjFirst", al::createActorFunction<NoteObj>},
    {"NoteObjFirst2D", al::createActorFunction<NoteObj2D>},
    {"NoteObjDirector", al::createActorFunction<NoteObjDirector>},
    {"Objex", al::createActorFunction<Objex>},
    {"OccludedEffectRequester", al::createActorFunction<al::OccludedEffectRequester>},
    {"OceanWave", al::createActorFunction<OceanWaveActor>},
    {"CloudOcean", al::createActorFunction<OceanWaveActor>},
    {"DemoCloudOcean", al::createActorFunction<OceanWaveActor>},
    {"OneMeshFixMapParts", al::createActorFunction<al::OneMeshFixMapParts>},
    {"OpeningStageStartDemo", al::createActorFunction<OpeningStageStartDemo>},
    {"PackunFire", al::createActorFunction<PackunFire>},
    {"PadRumblePoint", al::createActorFunction<PadRumblePoint>},
    {"PaintObj", al::createActorFunction<PaintObj>},
    {"PaulineAtCeremony", al::createActorFunction<PaulineAtCeremony>},
    {"PaulineAudience", al::createActorFunction<PaulineAudience>},
    {"PeachWorldHomeCastleCap", al::createActorFunction<PeachCastleCap>},
    {"PeachWorldGate", al::createActorFunction<PeachWorldGate>},
    {"PeachWorldMoatWater", al::createActorFunction<PeachWorldMoatWater>},
    {"PeachWorldTree", al::createActorFunction<PeachWorldTree>},
    {"Pecho", al::createActorFunction<Pecho>},
    {"Pen", al::createActorFunction<Pen>},
    {"PictureStageChange", al::createActorFunction<PictureStageChange>},
    {"PillarKeyMoveParts", al::createActorFunction<PillarKeyMoveParts>},
    {"PillarSwitchOpenMapParts", al::createActorFunction<PillarSwitchOpenMapParts>},
    {"PlayerMotionObserver", al::createActorFunction<PlayerMotionObserver>},
    {"PlayerStartObj", al::createActorFunction<PlayerStartObj>},
    {"PlayerSubjectiveWatchCheckObj", al::createActorFunction<PlayerSubjectiveWatchCheckObj>},
    {"PlayGuideBoard", al::createActorFunction<PlayGuideBoard>},
    {"PlayRecorder", al::createActorFunction<PlayRecorder>},
    {"PlayerStartObjNoLink", al::createActorFunction<PlayerStartObj>},
    {"PochiHintPhoto", al::createActorFunction<PochiHintPhoto>},
    {"Poetter", al::createActorFunction<Poetter>},
    {"PoleClimbParts", al::createActorFunction<PoleClimbParts>},
    {"PoleClimbPartsBreak", al::createActorFunction<PoleClimbParts>},
    {"PoleGrabCeil", al::createActorFunction<PoleGrabCeil>},
    {"PoleGrabCeilKeyMoveParts", al::createActorFunction<PoleGrabCeil>},
    {"PopnGenerator", al::createActorFunction<PopnGenerator>},
    {"LavaWorldPoster", al::createActorFunction<ReactionObjectPoster>},
    {"PosterCeremony", al::createActorFunction<ReactionObjectPoster>},
    {"PosterWedding", al::createActorFunction<ReactionObjectPoster>},
    {"ReactionObjectSkyRhythm", al::createActorFunction<ReactionObjectSkyRhythm>},
    {"PosterWatcher", al::createActorFunction<PosterWatcher>},
    {"PrePassCausticsLight", al::createActorFunction<PrePassCausticsLight>},
    {"PrePassLineLight", al::createActorFunction<al::PrePassLineLight>},
    {"PrePassPointLight", al::createActorFunction<al::PrePassPointLight>},
    {"PrePassProjLight", al::createActorFunction<al::PrePassProjLight>},
    {"PrePassProjOrthoLight", al::createActorFunction<al::PrePassProjOrthoLight>},
    {"PrePassSpotLight", al::createActorFunction<al::PrePassSpotLight>},
    {"ProjectRaceCheckPoint", al::createActorFunction<ProjectRaceCheckPoint>},
    {"Pyramid", al::createActorFunction<Pyramid>},
    {"QuestObj", al::createActorFunction<QuestObj>},
    {"RabbitGraph", al::createActorFunction<RabbitGraph>},
    {"RaceAudienceNpc", al::createActorFunction<RaceAudienceNpc>},
    {"RaceManGoal", al::createActorFunction<RaceManGoal>},
    {"RaceManRace", al::createActorFunction<RaceManRace>},
    {"RaceManStart", al::createActorFunction<RaceManStart>},
    {"RaceWatcher", al::createActorFunction<RaceWatcher>},
    {"RadiConRaceWatcher", al::createActorFunction<RadiConRaceWatcher>},
    {"RadioCassette", al::createActorFunction<RadioCassette>},
    {"RadiconNpc", al::createActorFunction<RadiconNpc>},
    {"Radish", al::createActorFunction<Radish>},
    {"RadishGold", al::createActorFunction<Radish>},
    {"RailDrawer", al::createActorFunction<RailDrawer>},
    {"RankingNpc", al::createActorFunction<RankingNpc>},
    {"ReactionObject", al::createActorFunction<ReactionObject>},
    {"CarBreakable", al::createActorFunction<ReactionObjectCarBreakable>},
    {"ReactionObjectDotCharacter", al::createActorFunction<ReactionObject>},
    {"ReflectBombGenerator", al::createActorFunction<ReflectBombGenerator>},
    {"RhythmSpotlight", al::createActorFunction<RhythmSpotlight>},
    {"RippleGeneratePoint", al::createActorFunction<al::RippleGeneratePoint>},
    {"RippleGenerateSquare", al::createActorFunction<al::RippleGeneratePoint>},
    {"RotateTarget", al::createActorFunction<RotateTarget>},
    {"RouletteSwitch", al::createActorFunction<RouletteSwitch>},
    {"RouteGuideArrow", al::createActorFunction<RouteGuideArrow>},
    {"RouteGuideRail", al::createActorFunction<RouteGuideRail>},
    {"RunAwayNpc", al::createActorFunction<RunAwayNpc>},
    {"SandGeyser", al::createActorFunction<SandGeyser>},
    {"SandWorldHomeLift", al::createActorFunction<SandWorldHomeLift>},
    {"SaucePan", al::createActorFunction<SaucePan>},
    {"SaveFlagCheckObj", al::createActorFunction<SaveFlagCheckObj>},
    {"ScenarioStartCameraAnim", al::createActorFunction<ScenarioStartCamera>},
    {"ScenarioStartCameraSimpleZoom", al::createActorFunction<ScenarioStartCamera>},
    {"ScenarioStartCameraRailMove", al::createActorFunction<ScenarioStartCamera>},
    {"Senobi", al::createActorFunction<Senobi>},
    {"SenobiGeneratePoint", al::createActorFunction<SenobiGeneratePoint>},
    {"SenobiMoveMapParts", al::createActorFunction<SenobiMoveMapParts>},
    {"SenobiMoveMapPartsConnector", al::createActorFunction<SenobiMoveMapPartsConnector>},
    {"SeBarrierObj", al::createActorFunction<al::SeBarrierObj>},
    {"SePlayObj", al::createActorFunction<al::SePlayObj>},
    {"SePlayObjWithSave", al::createActorFunction<SePlayObjWithSave>},
    {"SePlayRail", al::createActorFunction<al::SePlayRail>},
    {"SequentialSwitch", al::createActorFunction<SequentialSwitch>},
    {"SessionBgmCtrlObj", al::createActorFunction<SessionBgmCtrlObj>},
    {"SessionMayorNpc", al::createActorFunction<SessionMayorNpc>},
    {"SessionMusicianNpc", al::createActorFunction<SessionMusicianNpc>},
    {"Shibaken", al::createActorFunction<Shibaken>},
    {"ShibakenHomeShipInside", al::createActorFunction<Shibaken>},
    {"Shine", al::createActorFunction<Shine>},
    {"ShineWithAppearCamera", al::createActorFunction<Shine>},
    {"ShineChipWatcher", al::createActorFunction<ShineChipWatcher>},
    {"ShineDot", al::createActorFunction<Shine>},
    {"ShineFukankunWatchObj", al::createActorFunction<ShineFukankunWatchObj>},
    {"ShineTowerRocket", al::createActorFunction<ShineTowerRocket>},
    {"ShopBgmPlayer", al::createActorFunction<ShopBgmPlayer>},
    {"ShopMark", al::createActorFunction<ShopMark>},
    {"ShoppingWatcher", al::createActorFunction<ShoppingWatcher>},
    {"SignBoardDanger", al::createActorFunction<SignBoardDanger>},
    {"SignBoardLayoutTexture", al::createActorFunction<SignBoardLayoutTexture>},
    {"SkyFukankunZoomCapMessage", al::createActorFunction<SkyFukankunZoomCapMessage>},
    {"SkyWorldCloud", al::createActorFunction<SkyWorldCloud>},
    {"SkyWorldKoopaFire", al::createActorFunction<SkyWorldKoopaFire>},
    {"SkyWorldKoopaFrame", al::createActorFunction<SkyWorldKoopaFrame>},
    {"SkyWorldMiddleViewCloud", al::createActorFunction<SkyWorldMiddleViewCloud>},
    {"SignBoard", al::createActorFunction<SignBoard>},
    {"SnowWorldBigIcicle", al::createActorFunction<SnowWorldBigIcicle>},
    {"SnowWorldSequenceFlagCheckObj", al::createActorFunction<SnowWorldSequenceFlagCheckObj>},
    {"Sky", al::createActorFunction<al::Sky>},
    {"SmallWanderBoss", al::createActorFunction<SmallWanderBoss>},
    {"SneakingMan", al::createActorFunction<SneakingMan>},
    {"SnowManRaceNpc", al::createActorFunction<SnowManRaceNpc>},
    {"SnowVolume", al::createActorFunction<SnowVolume>},
    {"SnowVolumeEraser", al::createActorFunction<SnowVolumeEraser>},
    {"Souvenir", al::createActorFunction<Souvenir>},
    {"SouvenirDirector", al::createActorFunction<SouvenirDirector>},
    {"Special2KeyMoveLift", al::createActorFunction<Special2KeyMoveLift>},
    {"Special2KeyMoveParts", al::createActorFunction<Special2KeyMoveParts>},
    {"SphinxQuiz", al::createActorFunction<SphinxQuiz>},
    {"SphinxRide", al::createActorFunction<SphinxRide>},
    {"SphinxTaxiWatcher", al::createActorFunction<SphinxTaxiWatcher>},
    {"Squirrel", al::createActorFunction<Squirrel>},
    {"Stacker", al::createActorFunction<Stacker>},
    {"StackerCapWorldCtrl", al::createActorFunction<StackerCapWorldCtrl>},
    {"StageEventDemo", al::createActorFunction<StageEventDemo>},
    {"StageSwitchSelector", al::createActorFunction<StageSwitchSelector>},
    {"StageTalkDemoNpcCap", al::createActorFunction<StageTalkDemoNpcCap>},
    {"StageTalkDemoNpcCapMoonRock", al::createActorFunction<StageTalkDemoNpcCap>},
    {"Stake", al::createActorFunction<Stake>},
    {"Statue", al::createActorFunction<Statue>},
    {"StatueSnapMark", al::createActorFunction<StatueSnapMark>},
    {"SubActorLodFixPartsScenarioAction",
     al::createActorFunction<SubActorLodFixPartsScenarioAction>},
    {"SwitchAnd", al::createActorFunction<SwitchAnd>},
    {"SwitchKeyMoveMapParts", al::createActorFunction<al::SwitchKeyMoveMapParts>},
    {"TalkMessageInfoPoint", al::createActorFunction<CapMessagePlacement>},
    {"TalkMessageInfoPointSaveObj", al::createActorFunction<TalkMessageInfoPointSaveObj>},
    {"TalkNpc", al::createActorFunction<TalkNpc>},
    {"TalkNpcFreeze", al::createActorFunction<TalkNpcFreeze>},
    {"TalkNpcCapMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcCapManHero", al::createActorFunction<TalkNpc>},
    {"TalkNpcCityMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcCityManLow", al::createActorFunction<TalkNpc>},
    {"TalkNpcCityManSit", al::createActorFunction<TalkNpc>},
    {"TalkNpcCityMayor", al::createActorFunction<TalkNpc>},
    {"TalkNpcCollectBgm", al::createActorFunction<TalkNpc>},
    {"TalkNpcDesertMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcForestMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcForestManScrap", al::createActorFunction<TalkNpc>},
    {"TalkNpcKinopio", al::createActorFunction<TalkNpc>},
    {"TalkNpcKinopioBrigade", al::createActorFunction<TalkNpc>},
    {"TalkNpcKinopioMember", al::createActorFunction<TalkNpc>},
    {"TalkNpcLakeMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcLavaMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcLavaManCook", al::createActorFunction<TalkNpc>},
    {"TalkNpcLifeUpItemSeller", al::createActorFunction<TalkNpc>},
    {"TalkNpcRabbit", al::createActorFunction<TalkNpc>},
    {"TalkNpcSeaMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcSnowMan", al::createActorFunction<TalkNpc>},
    {"TalkNpcSnowManLeader", al::createActorFunction<TalkNpc>},
    {"TalkNpcSnowManRacer", al::createActorFunction<TalkNpc>},
    {"TalkPoint", al::createActorFunction<TalkPoint>},
    {"Tank", al::createActorFunction<Tank>},
    {"TankReviveCtrl", al::createActorFunction<TankReviveCtrl>},
    {"TaxiStop", al::createActorFunction<TaxiStop>},
    {"TextureReplaceScreen", al::createActorFunction<TextureReplaceScreen>},
    {"ThunderRenderRequester", al::createActorFunction<al::ThunderRenderRequester>},
    {"Togezo", al::createActorFunction<Togezo>},
    {"Togezo2D", al::createActorFunction<Togezo2D>},
    {"TokimekiMayorNpc", al::createActorFunction<TokimekiMayorNpc>},
    {"TrampleBush", al::createActorFunction<TrampleBush>},
    {"TrampleSwitch", al::createActorFunction<TrampleSwitch>},
    {"TrampleSwitchSave", al::createActorFunction<TrampleSwitch>},
    {"TrampleSwitchTimer", al::createActorFunction<TrampleSwitchTimer>},
    {"TransparentWall", al::createActorFunction<TransparentWall>},
    {"TreasureBox", al::createActorFunction<TreasureBox>},
    {"TreasureBoxKey", al::createActorFunction<TreasureBoxKey>},
    {"TreasureBoxSequentialDirector", al::createActorFunction<TreasureBoxSequentialDirector>},
    {"TRex", al::createActorFunction<TRex>},
    {"TRexForceScroll", al::createActorFunction<TRex>},
    {"TRexPatrol", al::createActorFunction<TRex>},
    {"TRexSleep", al::createActorFunction<TRex>},
    {"TRexScrollBreakMapParts", al::createActorFunction<TRexScrollBreakMapParts>},
    {"Tsukkun", al::createActorFunction<Tsukkun>},
    {"TsukkunHole", al::createActorFunction<TsukkunHole>},
    {"TwistChainList", al::createActorFunction<TwistChainList>},
    {"Utsubo", al::createActorFunction<Utsubo>},
    {"UtsuboWatcher", al::createActorFunction<UtsuboWatcher>},
    {"VocalMike", al::createActorFunction<VocalMike>},
    {"VolleyballBase", al::createActorFunction<VolleyballBase>},
    {"VolleyballNet", al::createActorFunction<VolleyballNet>},
    {"VolleyballNpc", al::createActorFunction<VolleyballNpc>},
    {"Wanwan", al::createActorFunction<Wanwan>},
    {"WanwanHole", al::createActorFunction<WanwanHole>},
    {"WaterAreaMoveModel", al::createActorFunction<al::WaterAreaMoveModel>},
    {"WaterfallWorldBigBreakableWall", al::createActorFunction<WaterfallWorldBigBreakableWall>},
    {"WaterfallWorldFallDownBridge", al::createActorFunction<WaterfallWorldFallDownBridge>},
    {"WaterfallWorldHomeCage", al::createActorFunction<WaterfallWorldBigBreakableWall>},
    {"WaterfallWorldWaterfall", al::createActorFunction<WaterfallWorldWaterfall>},
    {"WaterRoad", al::createActorFunction<WaterRoad>},
    {"WeightSwitch", al::createActorFunction<WeightSwitch>},
    {"WheelWaveSurfParts", al::createActorFunction<WheelWaveSurfParts>},
    {"WindBlowPuzzle", al::createActorFunction<WindBlowPuzzle>},
    {"WorldMapEarth", al::createActorFunction<WorldMapEarth>},
    {"WorldTravelingNpc", al::createActorFunction<WorldTravelingNpc>},
    {"WorldTravelingPeach", al::createActorFunction<WorldTravelingPeach>},
    {"WorldWarpHole", al::createActorFunction<WorldWarpHole>},
    {"Fastener", al::createActorFunction<Fastener>},
    {"FastenerObj", al::createActorFunction<FastenerObj>},
    {"AtmosScatterRequester", al::createActorFunction<al::AtmosScatterRequester>},
    {"BackHideParts", al::createActorFunction<al::BackHideParts>},
    {"BreakMapParts", al::createActorFunction<BreakMapParts>},
    {"CapRotateMapParts", al::createActorFunction<CapRotateMapParts>},
    {"ClockMapParts", al::createActorFunction<al::ClockMapParts>},
    {"ConveyerMapParts", al::createActorFunction<al::ConveyerMapParts>},
    {"FallMapParts", al::createActorFunction<al::FallMapParts>},
    {"FixMapParts", al::createActorFunction<al::FixMapParts>},
    {"FloaterMapParts", al::createActorFunction<al::FloaterMapParts>},
    {"FlowMapParts", al::createActorFunction<al::FlowMapParts>},
    {"GateMapParts", al::createActorFunction<al::GateMapParts>},
    {"KeyMoveMapParts", al::createActorFunction<al::KeyMoveMapParts>},
    {"KeyMoveMapPartsGenerator", al::createActorFunction<al::KeyMoveMapPartsGenerator>},
    {"PossessedMapParts", al::createActorFunction<HackMapParts>},
    {"Pukupuku", al::createActorFunction<Pukupuku>},
    {"PulseSwitch", al::createActorFunction<PulseSwitch>},
    {"RailCollision", al::createActorFunction<RailCollision>},
    {"RailMoveMapParts", al::createActorFunction<al::RailMoveMapParts>},
    {"RiseMapParts", al::createActorFunction<RiseMapParts>},
    {"ReactionMapParts", al::createActorFunction<ReactionMapParts>},
    {"RiseMapPartsHolder", al::createActorFunction<RiseMapPartsHolder>},
    {"RocketFlower", al::createActorFunction<RocketFlower>},
    {"RollingCubeMapParts", al::createActorFunction<al::RollingCubeMapParts>},
    {"RippleFixMapParts", al::createActorFunction<al::RippleFixMapParts>},
    {"RotateMapParts", al::createActorFunction<al::RotateMapParts>},
    {"SeesawMapParts", al::createActorFunction<al::SeesawMapParts>},
    {"SlideMapParts", al::createActorFunction<al::SlideMapParts>},
    {"SubActorLodMapParts", al::createActorFunction<al::SubActorLodMapParts>},
    {"SurfMapParts", al::createActorFunction<al::SurfMapParts>},
    {"SwingMapParts", al::createActorFunction<al::SwingMapParts>},
    {"SwitchDitherMapParts", al::createActorFunction<al::SwitchDitherMapParts>},
    {"SwitchKeepOnWatcher", al::createActorFunction<al::SwitchKeepOnWatcher>},
    {"SwitchOpenMapParts", al::createActorFunction<al::SwitchOpenMapParts>},
    {"VisibleSwitchMapParts", al::createActorFunction<al::VisibleSwitchMapParts>},
    {"WaveSurfMapParts", al::createActorFunction<WaveSurfMapParts>},
    {"WheelMapParts", al::createActorFunction<al::WheelMapParts>},
    {"WobbleMapParts", al::createActorFunction<al::WobbleMapParts>},
    {"WindBlowMapParts", al::createActorFunction<WindBlowMapParts>},
    {"Yoshi", al::createActorFunction<Yoshi>},
    {"YoshiFruit", al::createActorFunction<YoshiFruit>},
    {"YoshiFruitShineHolder", al::createActorFunction<YoshiFruitShineHolder>},
    {"Yukimaru", al::createActorFunction<Yukimaru>},
    {"YukimaruRacer", al::createActorFunction<YukimaruRacer>},
    {"YukimaruRacerTiago", al::createActorFunction<YukimaruRacerTiago>},
};

ProjectActorFactory::ProjectActorFactory() : ActorFactory("アクター生成") {
    initFactory(sProjectActorFactoryEntries);
}
