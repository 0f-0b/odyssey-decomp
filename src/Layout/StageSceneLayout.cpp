#include "Layout/StageSceneLayout.h"

#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Message/MessageHolder.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Layout/SimpleLayoutAppearWaitEnd.h"
#include "Library/Player/PlayerUtil.h"

#include "Item/ShineChipWatcherHolder.h"
#include "Layout/CoinCounter.h"
#include "Layout/CounterLifeCtrl.h"
#include "Layout/MapMini.h"
#include "Layout/PlayGuideBgm.h"
#include "Layout/PlayGuideCamera.h"
#include "Layout/ShineChipLayoutParts.h"
#include "Layout/ShineCounter.h"
#include "Util/StageInputFunction.h"

namespace {
NERVE_IMPL(StageSceneLayout, End);
NERVE_IMPL(StageSceneLayout, Appear);
NERVE_IMPL(StageSceneLayout, EndWithoutCoin);
NERVE_IMPL(StageSceneLayout, Wait);
NERVE_IMPL(StageSceneLayout, CoinCountAnim);
NERVE_IMPL(StageSceneLayout, ShineChipComplete);
NERVE_IMPL(StageSceneLayout, ShineCountAppear);

NERVES_MAKE_NOSTRUCT(StageSceneLayout, End);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(StageSceneLayout, Appear);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(StageSceneLayout, EndWithoutCoin);
NERVES_MAKE_NOSTRUCT(StageSceneLayout, Wait);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(StageSceneLayout, CoinCountAnim);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(StageSceneLayout, ShineChipComplete);
[[gnu::used]] NERVES_MAKE_NOSTRUCT(StageSceneLayout, ShineCountAppear);
}  // namespace

StageSceneLayout::StageSceneLayout(const char* name, const al::LayoutInitInfo& info,
                                   const al::PlayerHolder* player_holder,
                                   const al::SubCameraRenderer* sub_camera_renderer)
    : al::NerveStateBase(name), mPlayerHolder(player_holder) {
    initNerve(&End, 0);
    mCoinCounter = new CoinCounter("[シーン情報]コインカウンタ", info, true);
    mCoinCollectCounter = new CoinCounter("[シーン情報]コインコレクトカウンタ", info, false);
    mCounterLifeCtrl = new CounterLifeCtrl(info, mPlayerHolder, sub_camera_renderer);
    mShineCounter = new ShineCounter("[シーン情報]シャインカウンタ", info);
    mShineChipLayoutParts = new ShineChipLayoutParts(info, "CounterPiece");
    mPlayGuideCamera = new PlayGuideCamera("[シーン情報]カメラ操作レイアウト", info,
                                           al::getPlayerActor(mPlayerHolder, 0));
    mPlayGuideBgm = new PlayGuideBgm("[シーン情報]BGM再生レイアウト更新", info);

    mPlayGuideMenu = new al::SimpleLayoutAppearWaitEnd("[シーン情報]メニューガイド",
                                                       "PlayGuideMenu", info, nullptr, false);
    al::setPaneString(
        mPlayGuideMenu, "TxtGuideSh",
        al::getLayoutMessageString(mPlayGuideMenu, "PlayGuideMenu", "PlayGuideMenu_Guide"), 0);
    mPlayGuideMenu->kill();

    mMapMini = new MapMini(info, mPlayerHolder);

    if (rs::isExistShineChipWatcher(mShineChipLayoutParts) &&
        !rs::isAppearedShineChipShine(mShineChipLayoutParts)) {
        al::startAction(mShineChipLayoutParts, "Wait", nullptr);
    }

    mGamePadPlayStyleInfo = new GamePadPlayStyleInfo;
    rs::recordGamePadPlayStyleInfo(mGamePadPlayStyleInfo, mPlayGuideMenu);

    mLayoutActorKidsMode = new al::LayoutActor("キッズモード");
    al::initLayoutActor(mLayoutActorKidsMode, info, "KidsMode", nullptr);
    al::startAction(mLayoutActorKidsMode, "Wait", nullptr);
    mLayoutActorKidsMode->kill();

    kill();
}

void StageSceneLayout::exeAppear() {
    if (isActionEndAll())
        al::setNerve(this, &Wait);
}

bool StageSceneLayout::isActionEndAll() const {
    return mCoinCounter->isAliveAndActionEnd();
}

void StageSceneLayout::exeWait() {
    if (al::isFirstStep(this) && rs::isExistShineChipWatcher(mShineChipLayoutParts) &&
        !rs::isAppearedShineChipShine(mShineChipLayoutParts)) {
        al::startAction(mShineChipLayoutParts, "Wait", nullptr);
    }
}

void StageSceneLayout::exeCoinCountAnim() {}

void StageSceneLayout::exeShineChipComplete() {}

void StageSceneLayout::exeShineCountAppear() {}
