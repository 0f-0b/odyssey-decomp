#include "Library/Shader/Block/UniformBlock.h"

#include <detail/aglGPUMemBlockMgr.h>
#include <nvn/nvn_FuncPtrInline.h>

namespace al {

UniformBlock::UniformBlock() = default;

UniformBlockSetter::UniformBlockSetter(UniformBlock* block, s32 _w2) : mBlock(block), _0x8(_w2) {
    block->_0x41 = block->_0x78 = 1 - block->_0x78;
    u32 w8 = block->_0x41 * block->_0x44 * block->_0x1c + block->mMemAddr.mOffset;
    void* ptr = block->mMemAddr.mMemoryPool ? block->mMemAddr.mMemoryPool->map() : nullptr;
    block->_0x10 = static_cast<u8*>(ptr) + w8;
}

UniformBlockSetter::~UniformBlockSetter() {
    mBlock->flush(_0x8);
}

}  // namespace al
