#pragma once

#include "Library/Scene/ISceneObj.h"

namespace al {
class IUseSceneObjHolder;
class LiveActor;
}  // namespace al
class ShineChipWatcher;

class ShineChipWatcherHolder : public al::ISceneObj {
public:
    ShineChipWatcherHolder();
    void entry(ShineChipWatcher*);
    void notify(ShineChipWatcher*);
    void tryStartAppearShine();
    void getCurrentWatcher() const;
    void tryUpdateCurrentWatcher(const al::LiveActor*);

    const char* getSceneObjName() const override { return "シャインチップ全取得監視者 保持"; }
};

namespace ShineChipLocalFunction {
void tryCreateShineChipWatcherHolder(const al::IUseSceneObjHolder*);
void entryShineChipWatcher(ShineChipWatcher*);
void notifyShineChipGet(ShineChipWatcher*);
void notifyShineChipGet(const al::LiveActor*);
}  // namespace ShineChipLocalFunction

namespace rs {
bool isExistShineChipWatcher(const al::IUseSceneObjHolder*);
bool isCompleteShineChip(const al::IUseSceneObjHolder*);
void getShineChipCount(const al::IUseSceneObjHolder*);
void getCurrentShineChipWatcherIndex(const al::IUseSceneObjHolder*);
bool isCurrentShineChipWatcherTypeEmpty(const al::IUseSceneObjHolder*);
bool isAppearedShineChipShine(const al::IUseSceneObjHolder*);
void addDemoActorShineChipWatcher(const al::IUseSceneObjHolder*);
bool isEnableStartShineChipCompleteDemo(const al::IUseSceneObjHolder*);
void tryStartAppearShineChipShine(const al::IUseSceneObjHolder*);
bool isInAreaCurrentShineChipWatcher(const al::IUseSceneObjHolder*);
}  // namespace rs
