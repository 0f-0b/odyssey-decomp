#pragma once

#include <basis/seadTypes.h>

namespace sead {
class Heap;
}

namespace agl {
class DrawContext;
}

namespace nn::ui2d {
class DrawInfo;
}

namespace al {
class EffectSystem;
class ExecuteDirector;
class FontHolder;
class LayoutSystem;

class LayoutSystem {
public:
    LayoutSystem();
    void init();
    void initGraphicsResource();
    void initFont();
    void initEui();
    void tryFindFont(const char*) const;
    void getFontNamePair(s32) const;
    void prepareInitFontForChangeLanguage();
    void initFontForChangeLanguage();
    void reinitFont(sead::Heap*);
    void beginDraw() const;
    void endDraw() const;

private:
    void* _padding[0x6];
};

static_assert(sizeof(LayoutSystem) == 0x30);

class LayoutKit {
public:
    LayoutKit(FontHolder*);
    ~LayoutKit();

    void createCameraParamForIcon();
    void createExecuteDirector(s32);
    void createEffectSystem();
    void endInit();
    void update();
    void draw(const char*) const;
    void drawList(const char*, const char*) const;
    void setLayoutSystem(LayoutSystem*);
    void setDrawContext(agl::DrawContext*);

    void setEffectSystem(EffectSystem* effectSystem) { mEffectSystem = effectSystem; }

    ExecuteDirector* getExecuteDirector() const { return mExecuteDirector; }

    EffectSystem* getEffectSystem() const { return mEffectSystem; }

    nn::ui2d::DrawInfo* getDrawInfo() const { return mDrawInfo; }

    agl::DrawContext* getDrawContext() const { return mDrawContext; }

private:
    FontHolder* mFontHolder;
    ExecuteDirector* mExecuteDirector;
    EffectSystem* mEffectSystem;
    LayoutSystem* mLayoutSystem;
    agl::DrawContext* mDrawContext;
    nn::ui2d::DrawInfo* mDrawInfo;
};

static_assert(sizeof(LayoutKit) == 0x30);

}  // namespace al
