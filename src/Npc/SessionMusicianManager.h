#pragma once

#include <math/seadQuat.h>

#include "Library/LiveActor/LiveActor.h"

#include "Npc/SessionMusicianType.h"
#include "Scene/SceneObjFactory.h"

namespace al {
class PlacementInfo;
}
class SessionMayorNpc;
class SessionMusicianNpc;

class SessionMusicianManager : public al::LiveActor {
public:
    static constexpr s32 sSceneObjId = SceneObjID_SessionMusicianManager;

    SessionMusicianManager(const char* name);

    void initAfterPlacementSceneObj(const al::ActorInitInfo& info);
    void entryMusician(SessionMusicianNpc*);
    void isJoinedMusician() const;
    void getJoinedMusician() const;
    void isSubscribed(SessionMusicianType) const;
    void tryAppearPowerPlant();
    void findPowerPlant() const;
    void tryStartWarp(al::PlacementInfo*);
    void addDemoAllMusicians();

    void exeWait();
    void exeComplete();

private:
    void* _padding[0x5];
};

static_assert(sizeof(SessionMusicianManager) == 0x130);
