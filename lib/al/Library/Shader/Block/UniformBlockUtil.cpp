#include "Library/Shader/Block/UniformBlockUtil.h"

#include "Library/Shader/Block/UniformBlock.h"

namespace al {

UniformBlock* createUniformBlock(const UniformBlockLayout* layout, s32 layout_len, sead::Heap* heap,
                                 s32 _w3) {
    UniformBlock* result = new UniformBlock;
    declareUniformBlock(result, layout, layout_len, heap);
    result->create(heap, _w3, 1);
    return result;
}

void declareUniformBlock(agl::UniformBlock* block, const UniformBlockLayout* layout, s32 layout_len,
                         sead::Heap* heap) {
    block->startDeclare(layout_len, heap);
    for (s32 i = 0; i < layout_len; i++)
        block->declare_(layout[i]._0x4, layout[i]._0x8, 0, 1);
}

}  // namespace al
