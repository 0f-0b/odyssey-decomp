#pragma once

#include "Library/Scene/ISceneObj.h"

#include "Scene/SceneObjFactory.h"

namespace agl {
class DrawContext;
class SamplerLocation;
}  // namespace agl

class PlayerHackStartTexKeeper : public al::ISceneObj {
public:
    static constexpr s32 sSceneObjId = SceneObjID_PlayerHackStartTexKeeper;

    PlayerHackStartTexKeeper();
    ~PlayerHackStartTexKeeper() override;

    void clearHackStartTextureOnlyFirstTime(agl::DrawContext* draw_context);
    void activateHackStartTexture(agl::DrawContext* draw_context,
                                  const agl::SamplerLocation& sampler_location) const;

    const char* getSceneObjName() const override { return "キャプチャ開始テクスチャ保持"; }

private:
    void* _padding[0x7];
};

static_assert(sizeof(PlayerHackStartTexKeeper) == 0x40);
