#include "Npc/TalkNpcCap.h"

#include "Library/Base/StringUtil.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorResourceFunction.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Yaml/ByamlUtil.h"

// 0x1d6e7c8
static const CapType gShibakenCap = {"ShibakenCap", "ObjectData/ShibakenCap", "カウボーイ帽"};
// 0x1d6e7e0
static const CapType gShoppingCap = {"ShoppingCap", "ObjectData/ShoppingCap", "ショップ店員帽子"};
// 0x1e65e80
static const CapType gCapTypes[] = {
    {"PonchoCap", "ObjectData/MarioPonchoCap", "ポンチョ帽子"},
    {"GunmanCap", "ObjectData/MarioGunmanCap", "ガンマン帽子"},
    {"AlohaCap", "ObjectData/MarioAlohaCap", "アロハ帽子"},
    {"CookCap", "ObjectData/MarioCookCap", "コック帽子"},
    {"ExplorerCap", "ObjectData/MarioExplorerCap", "探検家帽子"},
    {"KingCap", "ObjectData/MarioKingCap", "王冠"},
    {"MechanicCap", "ObjectData/MarioMechanicCap", "メカニック帽子"},
    {"PainterCap", "ObjectData/MarioPainterCap", "画家帽子"},
    {"SailorCap", "ObjectData/MarioSailorCap", "水兵帽子"},
    {"SantaCap", "ObjectData/MarioSantaCap", "サンタ帽子"},
    {"VeilCap", "ObjectData/EnemyCapVeil", "ヴェール"},
    {"TailCoatCap", "ObjectData/MarioTailCoatCap", "紳士帽子"},
    {"TuxedoCap", "ObjectData/MarioTuxedoCap", "タキシード帽子"},
    {"Headphone", "ObjectData/Headphone", "ヘッドフォン"},
    {"Clown", "ObjectData/MarioClownCap", "ピエロ帽子"},
    {"Suit", "ObjectData/MarioSuitCap", "スーツ帽子"},
    {"ClownHair", "ObjectData/MarioClownHead", "ピエロアフロ"},
    {"SunVisor", "ObjectData/EnemyCapKiller", "サンバイザー"},
    {"ScientistCap", "ObjectData/MarioScientistCap", "博士帽子"},
    {"SnowSuitCap", "ObjectData/MarioSnowSuitCap", "防寒着帽子"},
    {"RacerCap", "ObjectData/MarioRacerCap", "レーサー帽子"},
    {"RaceManCap", "ObjectData/RaceManCap", "レースノコノコ帽子"},
    {"SpaceSuitCap", "ObjectData/MarioSpaceSuitCap", "宇宙服帽子"},
    {"VeilCap", "ObjectData/EnemyCapVeil", "ヴェール"},
    {"ArmorCap", "ObjectData/MarioArmorCap", "武者帽子"},
    {"MakerCap", "ObjectData/MarioMakerCap", "ビルダー帽子"},
    {"MarioCap", "ObjectData/MarioCap", "マリオ帽子"},
    {"PilotCap", "ObjectData/MarioPilotCap", "パイロット帽子"},
    {"SwimwearCap", "ObjectData/MarioSwimwearCap", "水泳帽子"},
    {"HappiCap", "ObjectData/MarioHappiCap", "法被帽子"},
    {"GolfCap", "ObjectData/MarioGolfCap", "ゴルフ帽子"},
    {"CowboyCap", "ObjectData/EnemyCapCowboy", "カウボーイ帽子"},
    {"64Cap", "ObjectData/Mario64Cap", "64帽子"},
    {"CaptainCap", "ObjectData/MarioCaptainCap", "海賊帽子"},
    {"ForestManCap", "ObjectData/ForestManCap", "森の民帽子"},
    gShoppingCap,
};

static bool tryGetCapType(s32* out, const al::ActorInitInfo& info) {
    return al::tryGetArg(out, info, "CapType") && *out < std::size(gCapTypes);
}

TalkNpcCap* TalkNpcCap::tryCreate(const al::LiveActor* actor, const al::ActorInitInfo& info) {
    s32 index = -1;
    if (!tryGetCapType(&index, info))
        return nullptr;
    const CapType* cap_type = &gCapTypes[index];
    TalkNpcCap* result = new TalkNpcCap(cap_type);
    al::initCreateActorNoPlacementInfo(result, info);
    result->initAttach(actor);
    return result;
}

void TalkNpcCap::initAttach(const al::LiveActor* actor) {
    mMtx = actor->getBaseMtx();
    if (al::isExistModelResourceYaml(actor, "InitPartsFixInfo", mCapType->id)) {
        al::ByamlIter iter{al::getModelResourceYaml(actor, "InitPartsFixInfo", mCapType->id)};
        const char* joint_name = nullptr;
        if (al::tryGetByamlString(&joint_name, iter, "JointName") && joint_name)
            mMtx = al::getJointMtxPtr(actor, joint_name);
        al::tryGetByamlV3f(&mLocalRotate, iter, "LocalRotate");
        al::tryGetByamlV3f(&mLocalTrans, iter, "LocalTrans");
        sead::Vector3f local_scale = {1, 1, 1};
        al::tryGetByamlV3f(&local_scale, iter, "LocalScale");
        mLocalScale = local_scale.x;
    }
    makeActorAlive();
}

TalkNpcCap* TalkNpcCap::createForAchievementNpc(const al::LiveActor* actor,
                                                const al::ActorInitInfo& info) {
    TalkNpcCap* result = new TalkNpcCap(&gCapTypes[4]);
    al::initCreateActorNoPlacementInfo(result, info);
    result->initAttach(actor);
    return result;
}

TalkNpcCap* TalkNpcCap::createForHintNpc(const al::LiveActor* actor,
                                         const al::ActorInitInfo& info) {
    TalkNpcCap* result = new TalkNpcCap(&gCapTypes[4]);
    al::initCreateActorNoPlacementInfo(result, info);
    result->initAttach(actor);
    return result;
}

TalkNpcCap* TalkNpcCap::createForShibaken(const al::LiveActor* actor,
                                          const al::ActorInitInfo& info) {
    s32 index = -1;
    if (!tryGetCapType(&index, info))
        return nullptr;
    const CapType* cap_type =
        al::isEqualString(gCapTypes[index].id, "CowboyCap") ? &gShibakenCap : &gCapTypes[index];
    TalkNpcCap* result = new TalkNpcCap(cap_type);
    al::initCreateActorNoPlacementInfo(result, info);
    result->initAttach(actor);
    return result;
}

TalkNpcCap* TalkNpcCap::createForShoppingNpc(const al::LiveActor* actor,
                                             const al::ActorInitInfo& info) {
    const CapType* cap_type = &gShoppingCap;
    TalkNpcCap* result = new TalkNpcCap(cap_type);
    al::initCreateActorNoPlacementInfo(result, info);
    al::initActorActionKeeper(result, info, cap_type->fileName, nullptr);
    result->initAttach(actor);
    return result;
}

TalkNpcCap* TalkNpcCap::createForShoppingNpcChromakey(const al::LiveActor* actor,
                                                      const al::ActorInitInfo& info) {
    const CapType* cap_type = &gShoppingCap;
    TalkNpcCap* result = new TalkNpcCap(cap_type);
    result->mIsChromakey = true;
    al::initCreateActorNoPlacementInfo(result, info);
    al::initActorActionKeeper(result, info, cap_type->fileName, nullptr);
    result->initAttach(actor);
    return result;
}

TalkNpcCap* TalkNpcCap::createForVolleyballNpc(const al::LiveActor* actor,
                                               const al::ActorInitInfo& info) {
    TalkNpcCap* result = new TalkNpcCap(&gCapTypes[2]);
    al::initCreateActorNoPlacementInfo(result, info);
    result->initAttach(actor);
    return result;
}

static void f_0x3e0bac(TalkNpcCap*, const sead::Matrix34f*, f32, const sead::Vector3f&,
                       const sead::Vector3f&, bool);

void TalkNpcCap::makeActorAlive() {
    al::LiveActor::makeActorAlive();
    f_0x3e0bac(this, mMtx, mLocalScale, mLocalTrans, mLocalRotate, mCapType == &gShoppingCap);
}

void TalkNpcCap::control() {
    f_0x3e0bac(this, mMtx, mLocalScale, mLocalTrans, mLocalRotate, mCapType == &gShoppingCap);
}

void TalkNpcCap::init(const al::ActorInitInfo& info) {
    al::initActorSceneInfo(this, info);
    al::initActorPoseTQSV(this);
    al::initActorSRT(this, info);
    al::initActorModelKeeper(this, info, mCapType->fileName, 1, nullptr);
    al::initActorClipping(this, info);
    al::invalidateClipping(this);

    if (mIsChromakey) {
        al::initExecutorUpdate(this, info, "ＮＰＣ装飾");
        al::initExecutorDraw(this, info, "ＮＰＣ[クロマキー]");
        al::initExecutorDraw(this, info, "Ｚプリパス[ＮＰＣクロマキー]");
    } else {
        al::initExecutorUpdate(this, info, "ＮＰＣ装飾");
        al::initExecutorDraw(this, info, "Ｚプリパス[ディザ]");
        bool deferred_opa;
        bool deferred_xlu;
        bool forward;
        bool indirect;
        al::getModelDrawCategoryFromShaderAssign(&deferred_opa, &deferred_xlu, &forward, &indirect,
                                                 this);
        if (deferred_opa && !deferred_xlu && !forward && !indirect)
            al::initExecutorDraw(this, info, "ＮＰＣ");
        else if (!deferred_opa && deferred_xlu && !forward && !indirect)
            al::initExecutorDraw(this, info, "ＮＰＣ[ディファード半透明]");
        else if (!deferred_opa && !deferred_xlu && forward && !indirect)
            al::initExecutorDraw(this, info, "ＮＰＣ[フォワード]");
        else if (!deferred_opa && !deferred_xlu && !forward && indirect)
            al::initExecutorDraw(this, info, "ＮＰＣ[インダイレクト]");
        else {
            if (deferred_opa)
                al::initExecutorDraw(this, info, "ＮＰＣ[ディファードのみ]");
            if (deferred_xlu)
                al::initExecutorDraw(this, info, "ＮＰＣ[ディファード半透明のみ]");
            if (forward)
                al::initExecutorDraw(this, info, "ＮＰＣ[フォワードのみ]");
            if (indirect)
                al::initExecutorDraw(this, info, "ＮＰＣ[インダイレクトのみ]");
        }
    }

    al::initExecutorModelUpdate(this, info);
    al::initActorMaterialCategory(this, info, "Obj");
}
