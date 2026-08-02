#pragma once

#include <common/aglUniformBlock.h>

namespace al {

struct UniformBlockLayout {
    s32 _0x0;
    agl::UniformBlock::Type _0x4;
    s32 _0x8;
};

class UniformBlock : public agl::UniformBlock {
public:
    UniformBlock();
    void swap();

private:
    friend class UniformBlockSetter;

    s32 _0x78 = 0;
};

static_assert(sizeof(UniformBlock) == 0x80);

class UniformBlockSetter {
public:
    UniformBlockSetter(UniformBlock* block, s32);
    ~UniformBlockSetter();

private:
    UniformBlock* mBlock;
    s32 _0x8;
};

}  // namespace al
