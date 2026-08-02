#pragma once

#include <basis/seadTypes.h>
#include <common/aglTextureEnum.h>
#include <gfx/seadColor.h>
#include <math/seadMatrix.h>

namespace agl {
class DrawContext;
class ShaderProgram;
class TextureData;
class TextureSampler;
}  // namespace agl

namespace al {
void createTexture(s32, s32, u8**);
void calcOrthoProjectedTexCoord(sead::Vector2f* out, const sead::Matrix34f& base_mtx,
                                const sead::Vector3f& trans, const sead::Vector3f& pos, f32 size_x,
                                f32 size_y);
void isInsideTexture(const sead::Vector2i&, const agl::TextureData*);
agl::TextureData* createAglTextureData(agl::TextureFormat, s32, s32, s32, agl::TextureAttribute);
void initAglTextureData(agl::TextureData*, agl::TextureFormat, s32, s32, s32,
                        agl::TextureAttribute);
void createAglTextureDataLinear(agl::TextureFormat, s32, s32, s32);
void initAglTextureDataLinear(agl::TextureData*, agl::TextureFormat, s32, s32, s32);
void destroyAglTextureAndImage(agl::TextureData**);
void drawColorCircle(agl::DrawContext*, const sead::Color4f&, const sead::Matrix34f&,
                     const sead::Matrix44f&);
void makeTextureDataFromArchive(agl::TextureData*, const char*, const char*, const char*);
void tryMakeTextureDataFromArchive(agl::TextureData*, const char*, const char*, const char*);
void getColor(const agl::TextureData*, s32, s32);
void getColorFromLinearTexture(const agl::TextureData*, s32, s32);
f32 getF32FromLinearTextureF16(const agl::TextureData*, s32, s32, s32, s32);
f32 getF32FromLinearTextureF32(const agl::TextureData*, s32, s32, s32, s32);
void calcNormalFromLinearTexture(sead::Vector3f*, const agl::TextureData*, s32, s32);
void activateSampler(agl::DrawContext*, const agl::TextureSampler*, const agl::ShaderProgram*,
                     const char*);
}  // namespace al
