#pragma once

#include <common/aglUniformBlock.h>

#include "Library/Shader/Block/UniformBlock.h"

namespace agl {
class ShaderProgram;
}

namespace al {
class GpuMemAllocator;

UniformBlock* createUniformBlock(const UniformBlockLayout* layout, s32 layout_len, sead::Heap* heap,
                                 s32);
void declareUniformBlock(agl::UniformBlock* block, const UniformBlockLayout* layout, s32 layout_len,
                         sead::Heap* heap);
void createUniformBlockInstanceNum(const UniformBlockLayout* layout, s32, s32, sead::Heap* heap,
                                   s32);
void allocUniformBlock(const UniformBlockLayout* layout, s32, GpuMemAllocator*, s32);
void setUniformBlockToShader(UniformBlock* block, agl::DrawContext*, const agl::ShaderProgram&,
                             const char*, s32);

}  // namespace al
