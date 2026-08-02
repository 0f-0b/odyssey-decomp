#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class BgmDataBase;

class BgmPlayObj : public LiveActor {
public:
    struct PlayParams {
        // TODO
    };

    BgmPlayObj(const char* name, bool = false);

    void init(const ActorInitInfo& info) override;
    void createShape(BgmDataBase* database);
    void init(const ActorInitInfo& info, const char*);
    void init(const ActorInitInfo& info, const char*, const char*, const char*);
    void initAfterPlacement() override;
    void appear() override;
    void kill() override;
    void stopBgm(s32);
    void movement() override;
    bool isEnableCalcSpeakerParam() const;
    void calc3DParams(bool);
    void finalize();
    void getDistanceFromSourceToListener();
    bool isPlayable() const;
    void activate(bool, bool, bool);
    void startBgm(bool, bool);
    void activate(const PlayParams& params, bool);
    void deactivate(bool, s32);

    void exeWaitOnSwitch();
    void exeWaitPlayStart();
    void exePlay();

private:
    void* _padding[0x1c];
};

static_assert(sizeof(BgmPlayObj) == 0x1e8);

}  // namespace al
