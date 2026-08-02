#include "System/GameSystem.h"

#include <gfx/nvn/seadGraphicsNvn.h>
#include <heap/seadExpHeap.h>

#include "Library/Application/ApplicationMessageReceiver.h"
#include "Library/Audio/AudioInfo.h"
#include "Library/Audio/System/AudioKeeperFunction.h"
#include "Library/Audio/System/AudioSystem.h"
#include "Library/Base/StringUtil.h"
#include "Library/Collision/CollisionCodeFunction.h"
#include "Library/Controller/GamePadSystem.h"
#include "Library/Controller/GamePadWaveVibrationPlayer.h"
#include "Library/Effect/EffectSystem.h"
#include "Library/File/FileUtil.h"
#include "Library/Framework/GameFrameworkNx.h"
#include "Library/Layout/LayoutSystem.h"
#include "Library/Memory/HeapUtil.h"
#include "Library/Message/MessageHolder.h"
#include "Library/Message/MessageSystem.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Network/AccountHolder.h"
#include "Library/Network/HtmlViewer.h"
#include "Library/Network/NetworkSystem.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/Sequence/SequenceInitInfo.h"
#include "Library/System/GameSystemInfo.h"
#include "Library/System/Performance.h"

#include "Sequence/HakoniwaSequence.h"
#include "Sequence/SequenceFactory.h"
#include "System/Application.h"
#include "System/GameConfigData.h"
#include "System/GameDataHolder.h"
#include "System/ProjectNfpDirector.h"

namespace nn::friends {
void Initialize();
}

namespace {
NERVE_IMPL(GameSystem, Play);

// TODO: Remove maybe_unused once this class is implemented and the nerves are used
[[maybe_unused]] NERVES_MAKE_STRUCT(GameSystem, Play);
}  // namespace

GameSystem::GameSystem() : al::NerveExecutor("ゲームシステム") {}

GameSystem::~GameSystem() {
    alAudioSystemFunction::destroyAudioResource(
        "システム常駐", mAudioLoadGroupInfoList,
        alAudioSystemFunction::getSeadAudioPlayerForSe(mAudioSystem),
        alAudioSystemFunction::getSeadAudioPlayerForBgm(mAudioSystem));

    mAudioSystem->removeAudiioFrameProccess(mWaveVibrationHolder);
    mAudioSystem->finalize();
    delete mAccountHolder;
    mGameSystemInfo->getNfpDirector()->finalize();
}

namespace {

class MessageSystemUser : public al::IUseMessageSystem {
public:
    MessageSystemUser(al::MessageSystem* message_system) : mMessageSystem(message_system) {}

    const al::MessageSystem* getMessageSystem() const override { return mMessageSystem; }

private:
    al::MessageSystem* mMessageSystem;
};

}  // namespace

void GameSystem::init() {
    mGameSystemInfo = new al::GameSystemInfo;
    mGameSystemInfo->setDrawSystemInfo(Application::instance()->getDrawSystemInfo());
    mStashedGameConfigData = new GameConfigData;
    initNerve(&NrvGameSystem.Play, 0);

    mAccountHolder = Application::instance()->getAccountHolder();
    nn::friends::Initialize();
    mNetworkSystem = new al::NetworkSystem(mAccountHolder->getUserHandle(), true);
    mNetworkSystem->requestSystemInitialize();
    mGameSystemInfo->setNetworkSystem(mNetworkSystem);

    mHtmlViewer = new al::HtmlViewer;
    mGameSystemInfo->setHtmlViewer(mHtmlViewer);

    mNfpDirector = new ProjectNfpDirector;
    mGameSystemInfo->setNfpDirector(mNfpDirector);
    mNfpDirector->initialize();

    sead::Heap* effect_system_heap = sead::ExpHeap::create(
        0x4600000, "EffectSystemHeap", nullptr, 8, sead::Heap::cHeapDirection_Forward, false);
    al::addNamedHeap(effect_system_heap, "EffectSystemHeap");
    al::EffectSystem* effect_system = al::EffectSystem::createSystem(
        mGameSystemInfo->getDrawSystemInfo()->drawContext, effect_system_heap);
    mGameSystemInfo->setEffectSystem(effect_system);

    al::LayoutSystem* layout_system = new al::LayoutSystem;
    layout_system->init();
    mGameSystemInfo->setLayoutSystem(layout_system);

    al::MessageSystem* message_system = new al::MessageSystem;
    mGameSystemInfo->setMessageSystem(message_system);

    al::AudioSystemInitInfo info;
    al::CollisionCodeList* material_code =
        alCollisionCodeFunction::tyrCreateCollisionCodeList("MaterialCode");
    al::CollisionCodeList* material_code_prefix =
        alCollisionCodeFunction::tyrCreateCollisionCodeList("MaterialCodePrefix");
    mGameSystemInfo->getEffectSystem()->setMaterialCodeList(material_code);
    mGameSystemInfo->getEffectSystem()->set_378(material_code_prefix);
    info._0x18 = 0.401f;
    info._0x1c = 1;
    info._0x38 = material_code;
    info._0x40 = material_code_prefix;
    info._0x11 = true;
    info._0x12 = true;
    info._0x28 = true;
    info._0x29 = true;
    info._0x0 = "SeData";
    info._0x8 = "BgmData";
    info._0x2c = 0.3f;
    info._0x30 = 1.061f;
    info._0x48 = 0x40000;
    sead::Heap* audio_heap = al::tryFindNamedHeap("AudioHeap");
    info._0x20 = audio_heap ? audio_heap->getFreeSize() : 0;
    mAudioSystem = new al::AudioSystem;
    mAudioSystem->init(info);
    al::setAudioPlayerToResourceSystem(
        alAudioSystemFunction::getSeadAudioPlayerForSe(mAudioSystem),
        alAudioSystemFunction::getSeadAudioPlayerForBgm(mAudioSystem));

    al::AudioResourceLoadGroupInfoList* audio_groups = new al::AudioResourceLoadGroupInfoList();
    audio_groups->init(2, 0);
    mAudioLoadGroupInfoList = audio_groups;

    al::AudioResourceLoadGroupInfo* system_group =
        new al::AudioResourceLoadGroupInfo("システム常駐", 1, 1);
    al::trySetAudioInfo(system_group->_0x8,
                        new al::AudioResourceLoadInfo("SeResourceStdSystem", false), false);
    al::trySetAudioInfo(system_group->_0x10, new al::AudioResourceLoadInfo("TestSE", false), false);
    al::trySetAudioInfo(audio_groups, system_group, false);

    al::AudioResourceLoadGroupInfo* other_group =
        new al::AudioResourceLoadGroupInfo("システム常駐以外の常駐", 5, 0);
    al::trySetAudioInfo(other_group->_0x8, new al::AudioResourceLoadInfo("SeResourceStd1st", false),
                        false);
    al::trySetAudioInfo(other_group->_0x8, new al::AudioResourceLoadInfo("SeResourceStd2nd", false),
                        false);
    al::trySetAudioInfo(other_group->_0x8, new al::AudioResourceLoadInfo("BgmResourceStd1st", true),
                        false);
    al::trySetAudioInfo(other_group->_0x8, new al::AudioResourceLoadInfo("BgmResourceStd2nd", true),
                        false);
    al::trySetAudioInfo(other_group->_0x8,
                        new al::AudioResourceLoadInfo("BgmResourceStdPrefetch", true), false);
    al::trySetAudioInfo(audio_groups, other_group, false);

    alAudioSystemFunction::loadAudioResource(
        "システム常駐", mAudioLoadGroupInfoList,
        alAudioSystemFunction::getSeadAudioPlayerForSe(mAudioSystem),
        alAudioSystemFunction::getSeadAudioPlayerForBgm(mAudioSystem));
    mGameSystemInfo->setAudioSystem(mAudioSystem);

    mGamePadSystem = new al::GamePadSystem;
    setPadName();
    mGameSystemInfo->setGamePadSystem(mGamePadSystem);
    mGamePadSystem->setAudioSystem(mAudioSystem);

    mWaveVibrationHolder = new al::WaveVibrationHolder(mGamePadSystem);
    mGameSystemInfo->setWaveVibrationHolder(mWaveVibrationHolder);
    mAudioSystem->addAudiioFrameProccess(mWaveVibrationHolder);

    mApplicationMessageReceiver = new al::ApplicationMessageReceiver;
    mApplicationMessageReceiver->init();
    mGameSystemInfo->setApplicationMessageReceiver(mApplicationMessageReceiver);

    al::setGpuPerformance(al::GpuPerformance::_2, nn::oe::PerformanceMode_Normal);
    al::createSequenceHeap();
    tryChangeSequence("HakoniwaSequence");
    nn::oe::FinishStartupLogo();
    Application::instance()->getGameFramework()->requestChangeUseGPU(true);
}

void GameSystem::movement() {
    mApplicationMessageReceiver->update();
    if (mApplicationMessageReceiver->isUpdatedOperationMode())
        mGamePadSystem->setInvalidateDisconnectFrame(600);
    mGamePadSystem->update();
    if (mNetworkSystem)
        mNetworkSystem->updateBeforeScene();
    if (mApplicationMessageReceiver->getPerformanceMode() == nn::oe::PerformanceMode_Boost) {
        static_cast<sead::GraphicsNvn*>(sead::Graphics::instance())
            ->setDisplayBufferWindowCrop(0, 0, 1600, 900);
        Application::instance()->getGameFramework()->setDocked(true);
        mGameSystemInfo->getDrawSystemInfo()->isDocked = true;
    } else {
        static_cast<sead::GraphicsNvn*>(sead::Graphics::instance())
            ->setDisplayBufferWindowCrop(0, 0, 1280, 720);
        Application::instance()->getGameFramework()->setDocked(false);
        mGameSystemInfo->getDrawSystemInfo()->isDocked = false;
    }
    updateNerve();
    mAudioSystem->update();
    if (mNetworkSystem)
        mNetworkSystem->updateAfterScene();
    if (!mSequence->isAlive()) {
        if (al::isEqualString("HakoniwaSequence", mSequence->getName())) {
            GameDataHolder* game_data_holder =
                static_cast<HakoniwaSequence*>(mSequence)->getGameDataHolder();
            mIsSeparatePlay = game_data_holder->isSeparatePlay();
            mHasStashedGameConfigData = true;
            *mStashedGameConfigData = *game_data_holder->getGameConfigData();
        }
        mGameSystemInfo->getLayoutSystem()->prepareInitFontForChangeLanguage();
        al::removeResourceCategory("常駐[ローカライズ]");
        al::findNamedHeap("LocalizeResourceHeap")->freeAll();
        al::addResourceCategory("常駐[ローカライズ]", 80,
                                al::findNamedHeap("LocalizeResourceHeap"));
        al::clearFileLoaderEntry();
        al::createCategoryResourceAll("常駐[ローカライズ]");
        mGameSystemInfo->getLayoutSystem()->initFontForChangeLanguage();
        mGameSystemInfo->getMessageSystem()->initMessageForChangeLanguage();
        tryChangeSequence("HakoniwaSequence");
    }
}

void GameSystem::setPadName() {
    MessageSystemUser user{mGameSystemInfo->getMessageSystem()};
    mGamePadSystem->setPadName(
        0, al::getSystemMessageString(&user, "ControllerApplet", "SeparatePlayer1"));
    mGamePadSystem->setPadName(
        1, al::getSystemMessageString(&user, "ControllerApplet", "SeparatePlayer2"));
}

bool GameSystem::tryChangeSequence(const char* name) {
    if (mSequence) {
        if (!mSequence->isDisposable())
            return false;
        delete mSequence;
        mSequence = nullptr;
        al::freeAllSequenceHeap();
    }
    setPadName();
    sead::ScopedCurrentHeapSetter heap_setter(al::getSequenceHeap());
    al::Sequence* sequence = SequenceFactory::createSequence(name);
    if (!sequence)
        return false;
    al::SequenceInitInfo info{mGameSystemInfo};
    sequence->init(info);
    mSequence = sequence;
    if (al::isEqualString(name, "HakoniwaSequence")) {
        GameDataHolder* game_data_holder =
            static_cast<HakoniwaSequence*>(mSequence)->getGameDataHolder();
        game_data_holder->setSeparatePlay(mIsSeparatePlay);
        if (mHasStashedGameConfigData) {
            *game_data_holder->getGameConfigData() = *mStashedGameConfigData;
            mHasStashedGameConfigData = false;
        }
    }
    return true;
}

void GameSystem::drawMain() {
    Application::instance()->getGameFramework()->clearFrameBuffer();
    mGameSystemInfo->getLayoutSystem()->beginDraw();
    if (mSequence)
        mSequence->drawMain();
    mGameSystemInfo->getLayoutSystem()->endDraw();
}

void GameSystem::exePlay() {
    mNfpDirector->update();
    if (mSequence)
        mSequence->update();
}
