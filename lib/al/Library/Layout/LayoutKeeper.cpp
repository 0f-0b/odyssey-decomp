#include "Library/Layout/LayoutKeeper.h"

#include <nn/ui2d/Layout.h>

#include "Library/Base/StringUtil.h"
#include "Library/Layout/LayoutPaneGroup.h"
#include "Library/Message/CustomTagProcessor.h"

namespace al {

LayoutKeeper::LayoutKeeper() = default;

void LayoutKeeper::initScreen(eui::Screen* screen) {
    mScreen = screen;
}

void LayoutKeeper::initLayout(nn::ui2d::Layout* layout, LayoutResource* resource) {
    mLayout = layout;
}

void LayoutKeeper::initDrawInfo(nn::ui2d::DrawInfo* draw_info) {
    mDrawInfo = draw_info;
}

void LayoutKeeper::initTagProcessor(CustomTagProcessor* tag_processor) {
    mTagProcessor = tag_processor;
    mLayout->SetTagProcessor(tag_processor);
}

LayoutPaneGroup* LayoutKeeper::getGroup(const char* name) const {
    for (s32 i = 0; i < mGroupNum; i++)
        if (isEqualString(name, mGroups[i]->getName()))
            return mGroups[i];
    return nullptr;
}

LayoutPaneGroup* LayoutKeeper::getGroup(s32 index) const {
    return mGroups[index];
}

s32 LayoutKeeper::getGroupNum() const {
    return mGroupNum;
}

}  // namespace al
