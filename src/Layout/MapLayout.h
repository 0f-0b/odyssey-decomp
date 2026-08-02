#pragma once

#include <math/seadMatrix.h>

#include "Library/Layout/LayoutActor.h"
#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace al {
class LayoutInitInfo;
class PlayerHolder;
}  // namespace al
class MapIconInfo;
class MapIconLayout;

enum IconType {};

class MapLayout : public al::LayoutActor, public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_MapLayout;

    MapLayout(const al::LayoutInitInfo&, const al::PlayerHolder*, s32);
    void changePrintWorld(s32);
    void loadTexture();
    void reset();
    void appear() override;
    void moveFocusLayout(const sead::Vector3f&, const sead::Vector2f&);
    void updateST();
    void addAmiiboHint();
    void appearAmiiboHint();
    void end();
    void updatePlayerPosLayout();
    void appearWithHint();
    void appearMoonRockDemo(s32);
    void appearCollectionList();
    void isEnd() const;
    void isEnableCheckpointWarp() const;
    void changeOut(bool);
    void changeIn(bool);
    void control() override;
    void updateLine(al::LayoutActor*);
    void exeAppear();
    void appearParts(bool);
    void startNumberAction();
    void exeWait();
    void exeHintInitWait();
    void exeHintAppear();
    void calcSeaOfTreeIconPos(sead::Vector3f*);
    void setLocalTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType,
                                bool);
    void calcMapTransAndAppear(MapIconLayout*, MapIconInfo*, const sead::Vector3f&, IconType, bool);
    void exeHintDecideIconAppear();
    void exeHintDecideIconWait();
    void exeHintPressDecide();
    void exeEnd();
    void exeChangeOut();
    void scroll(const sead::Vector2f&);
    void addSize(const sead::Vector2f&);
    void isAppear() const;
    void getViewProjMtx() const;
    void getProjMtx() const;
    void updateIconLine(al::LayoutActor*, const sead::Vector3f&, const sead::Vector2f&);
    void focusIcon(const MapIconInfo*);
    void lostFocusIcon(MapIconLayout*);
    void tryCalcNorthDir(sead::Vector3f*);

    const char* getSceneObjName() const override { return "マップレイアウト"; }

private:
    void* _padding_0x138[0x2c];
};

static_assert(sizeof(MapLayout) == 0x298);

namespace rs {
void calcTransOnMap(sead::Vector2f*, const sead::Vector3f&, const sead::Matrix44f&,
                    const sead::Vector2f&, f32, f32);
}
