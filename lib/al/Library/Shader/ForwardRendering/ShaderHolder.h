#pragma once

#include <heap/seadDisposer.h>

#include "Library/HostIO/HioNode.h"

namespace al {

class ShaderHolder : public HioNode {
    SEAD_SINGLETON_DISPOSER(ShaderHolder)

public:
    s64 getShaderProgram(const char*) const;
    // incomplete

private:
    // missing
};

}  // namespace al
