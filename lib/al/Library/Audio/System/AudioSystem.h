#pragma once

#include <basis/seadTypes.h>

namespace aal {
class IAudioFrameProcess;
}

namespace al {
class AudioEffectDataBase;
class AudioResourceLoadingInfo;
class AudioResourcePlayerKeeper;
class AudioSystemDebug;
class BgmDataBase;
class BgmMusicalInfo;
class CollisionCodeList;
class SeDataBase;
struct AudioSystemInfo;

struct AudioSystemInitInfo {
    const char* _0x0 = nullptr;
    const char* _0x8 = nullptr;
    bool _0x10 = true;
    bool _0x11 = true;
    bool _0x12 = false;
    f32 _0x14 = 1;
    f32 _0x18 = 1;
    f32 _0x1c = 1;
    s32 _0x20 = -1;
    s32 _0x24 = 0;
    bool _0x28 = false;
    bool _0x29 = false;
    f32 _0x2c = 1;
    f32 _0x30 = 1;
    CollisionCodeList* _0x38 = nullptr;
    CollisionCodeList* _0x40 = nullptr;
    s32 _0x48 = 0;
};

class AudioSystem {
public:
    AudioSystem();

    void init(const AudioSystemInitInfo& info);
    void initDataBase();
    void initResourcePlayer(const AudioSystemInitInfo& info);
    void applyDeviceVolume();
    void updateHWOutputSetting();
    void changeFinalMixInputBgmChVolume();
    void initDebugModule(const AudioSystemInitInfo& info);
    void initSystemInfo();
    void update();
    void finalize();
    void addAudiioFrameProccess(aal::IAudioFrameProcess* process);
    void removeAudiioFrameProccess(aal::IAudioFrameProcess* process);
    void pauseSystemImmediately(bool, const char*, bool);

    AudioSystemInfo* getAudioSystemInfo() const { return mAudioSystemInfo; }

private:
    AudioResourcePlayerKeeper* mAudioResourcePlayerKeeper;
    AudioResourceLoadingInfo* mAudioResourceLoadingInfo;
    SeDataBase* mSeDataBase;
    BgmDataBase* mBgmDataBase;
    BgmMusicalInfo* mBgmMusicalInfo;
    AudioSystemInfo* mAudioSystemInfo;
    AudioSystemDebug* mAudioSystemDebug;
    f32 mMasterVolume;
    f32 mTvOutputVolume;
    f32 mConsoleVolume;
    AudioEffectDataBase* mAudioEffectDataBase;
    char _padding[0x90];
};

static_assert(sizeof(AudioSystem) == 0xe0);

}  // namespace al
