#pragma once

#include "Library/Nerve/NerveStateBase.h"

namespace al {
class LayoutActor;
class LayoutInitInfo;
class PlayerHolder;
class SimpleLayoutAppearWaitEnd;
class SubCameraRenderer;
}  // namespace al
class CoinCounter;
class CounterLifeCtrl;
class MapMini;
class PlayGuideBgm;
class PlayGuideCamera;
class ShineChipLayoutParts;
class ShineCounter;
struct GamePadPlayStyleInfo;

class StageSceneLayout : public al::NerveStateBase {
public:
    StageSceneLayout(const char* name, const al::LayoutInitInfo& info,
                     const al::PlayerHolder* player_holder,
                     const al::SubCameraRenderer* sub_camera_renderer);
    void startActionAll(const char*);
    void control() override;
    void updatePlayGuideMenuText();
    void setDirtyFlagForPlayGuideMenu();
    void start();
    void updateCounterParts();
    void tryAppearCoinCollectCounter();
    void startOnlyCoin(bool);
    void endWithoutCoin(bool);
    void end();
    bool isEnd() const;
    bool isWait() const;
    bool isActive() const;
    bool isEndLifeDemo() const;
    bool tryStartLifeDemo();
    void startCoinCountAnim(s32);
    void startCoinCollectCountAnim(s32);
    void appearCoinCounterForDemo();
    bool isEndCoinCountAnim() const;
    bool isEndShineChipCompleteAnim() const;
    void startShineChipCompleteAnim();
    void endShineChipCompleteAnim();
    bool tryStartDemoGetLifeMaxUpItem(bool);
    bool isEndDemoGetLifeMaxUpItem() const;
    void killShineCount();
    void appearShineCountWait();
    void startCloset();
    void endCloset();
    void missEnd();
    void appearPlayGuideCamera();
    void exeAppear();
    bool isActionEndAll() const;
    void exeWait();
    void exeEnd();
    void exeEndWithoutCoin();
    void exeCoinCountAnim();
    void exeShineChipComplete();
    void exeShineCountAppear();
    void updateLifeCounter();
    void updateKidsModeLayout();
    void startShineCountAnim(bool isAddTenShines);
    bool isEndShineCountAnim() const;

private:
    CoinCounter* mCoinCounter = nullptr;
    CounterLifeCtrl* mCounterLifeCtrl = nullptr;
    ShineCounter* mShineCounter = nullptr;
    CoinCounter* mCoinCollectCounter = nullptr;
    ShineChipLayoutParts* mShineChipLayoutParts = nullptr;
    PlayGuideCamera* mPlayGuideCamera = nullptr;
    PlayGuideBgm* mPlayGuideBgm = nullptr;
    MapMini* mMapMini = nullptr;
    const al::PlayerHolder* mPlayerHolder;
    bool _60 = false;
    al::SimpleLayoutAppearWaitEnd* mPlayGuideMenu = nullptr;
    GamePadPlayStyleInfo* mGamePadPlayStyleInfo = nullptr;
    al::LayoutActor* mLayoutActorKidsMode = nullptr;
};

static_assert(sizeof(StageSceneLayout) == 0x80);
