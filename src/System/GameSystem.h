#pragma once

#include "Library/Audio/AudioInfo.h"
#include "Library/Nerve/NerveExecutor.h"

namespace al {
class AccountHolder;
class ApplicationMessageReceiver;
class AudioSystem;
class GamePadSystem;
class GameSystemInfo;
class HtmlViewer;
class NetworkSystem;
class Sequence;
class WaveVibrationHolder;
}  // namespace al
class GameConfigData;
class ProjectNfpDirector;

class GameSystem : public al::NerveExecutor {
public:
    GameSystem();
    ~GameSystem() override;

    virtual void init();
    virtual void movement();
    void setPadName();
    bool tryChangeSequence(const char* name);
    void drawMain();

    void exePlay();

private:
    al::Sequence* mSequence = nullptr;
    al::GameSystemInfo* mGameSystemInfo = nullptr;
    al::AudioSystem* mAudioSystem = nullptr;
    al::AudioResourceLoadGroupInfoList* mAudioLoadGroupInfoList = nullptr;
    al::AccountHolder* mAccountHolder = nullptr;
    al::NetworkSystem* mNetworkSystem = nullptr;
    void* _0x40 = nullptr;
    al::HtmlViewer* mHtmlViewer = nullptr;
    ProjectNfpDirector* mNfpDirector = nullptr;
    al::GamePadSystem* mGamePadSystem = nullptr;
    al::ApplicationMessageReceiver* mApplicationMessageReceiver = nullptr;
    al::WaveVibrationHolder* mWaveVibrationHolder = nullptr;
    bool mIsSeparatePlay = false;
    GameConfigData* mStashedGameConfigData = nullptr;
    bool mHasStashedGameConfigData = false;
};

static_assert(sizeof(GameSystem) == 0x88);
