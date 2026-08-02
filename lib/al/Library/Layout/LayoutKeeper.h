#pragma once

#include <basis/seadTypes.h>

namespace nn::ui2d {
class Layout;
class DrawInfo;
}  // namespace nn::ui2d

namespace eui {
class Screen;
}

namespace al {
class LayoutPaneGroup;
class LayoutResource;
class CustomTagProcessor;

class LayoutKeeper {
public:
    LayoutKeeper();

    void initScreen(eui::Screen* screen);
    void initLayout(nn::ui2d::Layout* layout, LayoutResource* resource);
    void initDrawInfo(nn::ui2d::DrawInfo* draw_info);
    void initTagProcessor(CustomTagProcessor* tag_processor);
    LayoutPaneGroup* getGroup(const char* name) const;
    LayoutPaneGroup* getGroup(s32 index) const;
    s32 getGroupNum() const;
    void calcAnim(bool);
    void draw();

    nn::ui2d::Layout* getLayout() const { return mLayout; }

private:
    CustomTagProcessor* mTagProcessor = nullptr;
    nn::ui2d::DrawInfo* mDrawInfo = nullptr;
    nn::ui2d::Layout* mLayout = nullptr;
    LayoutPaneGroup** mGroups = nullptr;
    s32 mGroupNum = 0;
    eui::Screen* mScreen = nullptr;
    s64 _0x30 = 0;
};

static_assert(sizeof(LayoutKeeper) == 0x38);
}  // namespace al
