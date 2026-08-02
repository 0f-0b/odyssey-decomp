#pragma once

#include <basis/seadTypes.h>

#include "Library/HostIO/HioNode.h"

namespace aal {

class IAudioFrameProcess {
public:
    virtual ~IAudioFrameProcess() = default;

protected:
    virtual void audioFrameProcess_() = 0;
};

}  // namespace aal

namespace al {
class GamePadSystem;

class WaveVibrationHolder : public HioNode, public aal::IAudioFrameProcess {
public:
    WaveVibrationHolder(const GamePadSystem*);
    void loadData();
    void update();
    void findWaveVibrationData(const char*) const;
    void findPlayableVibrationPlayer(s32) const;
    void findId(s32) const;
    void stopAll();
    void getUsePadNum() const;
    void pause();
    void endPause();
    void startVibrationDirectValue(s32, f32, f32, f32, f32, f32, f32);
    void stopVibrationDirectValue(s32);

protected:
    void audioFrameProcess_() override;

private:
    void* _padding[0x1b];
};

static_assert(sizeof(WaveVibrationHolder) == 0xe0);

}  // namespace al
