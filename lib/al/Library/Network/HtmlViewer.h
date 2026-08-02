#pragma once

#include <prim/seadSafeString.h>

namespace al {

class HtmlViewer {
public:
    HtmlViewer();
    void call(const char*, sead::BufferedSafeString*) const;
};

static_assert(sizeof(HtmlViewer) == 0x1);

}  // namespace al
