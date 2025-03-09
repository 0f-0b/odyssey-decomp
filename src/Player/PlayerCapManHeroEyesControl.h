#pragma once

namespace al {
class LiveActor;
}

class PlayerCapManHeroEyesControl {
public:
    PlayerCapManHeroEyesControl(const char*, al::LiveActor*, al::LiveActor*);
    void start();
    void fastStart();
    void end();
    void startPuppet();
    void endPuppet();
    void kill();
    void update();
    void getPuppetEye() const;
    void isAppear() const;
    void isDisappear() const;
    void isDemo() const;
    void isDeadCap() const;
    void requestWaitAnimChange(const char*);
    void exeAppear();
    void exeWait();
    void exeDisappear();
    void exeDead();
    void exeDemo();
};
