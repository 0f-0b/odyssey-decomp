#pragma once

#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>

namespace nn::g3d {
class MaterialObj;
class ModelObj;
class ResMaterial;
class ResMesh;
class ResModel;
class ResShadingModel;
class ResShape;
class ShadingModelObj;
class ShapeObj;
class SkeletonObj;
class ViewVolume;
}  // namespace nn::g3d

namespace sead {
class GraphicsContext;
class GraphicsContextMRT;
class Heap;
}  // namespace sead

namespace agl {
class DrawContext;
}

namespace al {
class GraphicsSystemInfo;
class MeshDrawerTable;
class ModelAdditionalInfo;
class ModelCtrl;
class ModelKeeper;
class ModelShaderAssign;
class ShaderHolder;
struct CulledIndexCreateArg;
struct GraphicsContextDrawInfo;

enum class DeferredXluType {};
}  // namespace al

namespace alModelFunction {
void updateRenderMaterialUbo(nn::g3d::MaterialObj*);
void updateRenderCloudLayerUbo(nn::g3d::MaterialObj*);
void setDeferredGraphicsContext(sead::GraphicsContextMRT*, const nn::g3d::MaterialObj*,
                                const al::GraphicsContextDrawInfo&);
bool isShaderAssignXlu(const nn::g3d::MaterialObj*);
bool isShaderAssignAlphaMask(const nn::g3d::MaterialObj*);
bool isDeferredXluTypeBlendLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeBlendBcNrmLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeBlendBcLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeAddBcLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeAddLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeMulLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeMulAddLbuf(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeMulBc(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeFootPrint(const nn::g3d::ResMaterial*);
bool isDeferredXluTypeBlendAll(const nn::g3d::ResMaterial*);
void setForwardGraphicsContext(sead::GraphicsContext*, const nn::g3d::MaterialObj*,
                               const al::GraphicsContextDrawInfo&);
void prepareModelShapeDrawDeferredGraphicsContext(agl::DrawContext*, const al::ModelCtrl*, s32);
void prepareModelShapeDrawDeferredXluGraphicsContext(agl::DrawContext*, const al::ModelCtrl*, s32,
                                                     al::DeferredXluType, bool);
void drawModelShape(agl::DrawContext*, const nn::g3d::SkeletonObj*, const nn::g3d::MaterialObj*,
                    const nn::g3d::ShapeObj*, const nn::g3d::ShadingModelObj*,
                    const al::ModelShaderAssign*, const nn::g3d::ViewVolume*, s32, s32);
bool isExistBoundingNode(const nn::g3d::ResShape*);
void createModelShaderAssign(const nn::g3d::ShapeObj*, const nn::g3d::MaterialObj*,
                             nn::g3d::ResShadingModel*, s32, const char* const*,
                             const char* const*);
void createMeshDrawerTableFromAssignShader(const al::GraphicsSystemInfo*, al::ModelCtrl*, s32);
void createMeshDrawerDisplayListGraphicsContextInvalidate(const al::MeshDrawerTable*,
                                                          const al::ModelKeeper*, sead::Heap*);
void createMeshDrawerDisplayList(const al::MeshDrawerTable*, const al::ModelKeeper*, sead::Heap*);
void createMeshDrawerDisplayListMRT(const al::MeshDrawerTable*, const al::ModelKeeper*,
                                    sead::Heap*);
void createMeshDrawerDisplayListShadow(const al::MeshDrawerTable*, const al::ModelKeeper*,
                                       sead::Heap*);
bool isShaderAssignAlphaMask(const nn::g3d::ModelObj*, s32);
void createMeshDrawerDisplayListDepth(const al::MeshDrawerTable*, const al::ModelKeeper*, bool,
                                      sead::Heap*);
bool isShaderAssignDisplacement(const nn::g3d::ModelObj*, s32);
void createMeshDrawerDisplayListDepthWithCulling(const al::MeshDrawerTable*, const al::ModelKeeper*,
                                                 bool, sead::Heap*);
void createMeshDrawerDisplayListCulling(const al::MeshDrawerTable*, const al::ModelKeeper*,
                                        sead::Heap*);
bool isShaderAssignMaterialShader(const nn::g3d::ModelObj*, s32);
bool isShaderAssignMaterialShader(const nn::g3d::MaterialObj*);
bool isShaderAssignDisplacement(const nn::g3d::MaterialObj*);
bool isShaderAssignOpa(const nn::g3d::ModelObj*, s32);
bool isShaderAssignOpa(const nn::g3d::MaterialObj*);
bool isShaderAssignIndirect(const nn::g3d::MaterialObj*);
bool isShaderAssignXlu(const nn::g3d::ModelObj*, s32);
bool isShaderAssignDeferred(const nn::g3d::ModelObj*, s32);
bool isShaderAssignDeferred(const nn::g3d::MaterialObj*);
bool isShaderAssignForward(const nn::g3d::ModelObj*, s32);
bool isShaderAssignForward(const nn::g3d::MaterialObj*);
bool isShaderAssignCubemap(const nn::g3d::ModelObj*, s32);
bool isShaderAssignCubemap(const nn::g3d::MaterialObj*);
bool isShaderAssignIndirect(const nn::g3d::ModelObj*, s32);
bool isShaderAssignIndirect(const nn::g3d::ResMaterial*);
bool isShaderAssignUseRipple(const nn::g3d::ModelObj*, s32);
bool isShaderAssignUseRipple(const nn::g3d::MaterialObj*);
bool isShaderUseFrameBuffer(const nn::g3d::ResMaterial*);
bool isShaderUseGBufferBc(const nn::g3d::ResMaterial*);
bool isShaderUseGBufferNormal(const nn::g3d::ResMaterial*);
bool isShaderUseGBufferRoughness(const nn::g3d::ResMaterial*);
bool isShaderUseGBufferMetalness(const nn::g3d::ResMaterial*);
bool isShaderUseEmission(const nn::g3d::MaterialObj*);
bool isShaderUseExposure(const nn::g3d::ResMaterial*);
bool isShaderUseRefractTex(const nn::g3d::ResMaterial*);
bool isShaderUseMirror(const nn::g3d::ResMaterial*);
bool isShaderUseRefractTex(const nn::g3d::MaterialObj*);
bool isShaderUseRefractCubemap(const nn::g3d::MaterialObj*);
bool isShaderUseRefractCubemap(const nn::g3d::ResMaterial*);
bool isShaderUseRefractGem(const nn::g3d::MaterialObj*);
bool isShaderUseSSS(const nn::g3d::MaterialObj*);
bool isShaderUseIrradianePixel(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaScale(const nn::g3d::MaterialObj*);
bool isShaderUseFlowMap(const nn::g3d::MaterialObj*);
bool isShaderUseTranslucent(const nn::g3d::MaterialObj*);
bool isShaderUseBaseColorTexSrt(const nn::g3d::MaterialObj*);
bool isShaderUseBaseColorModelUv1(const nn::g3d::MaterialObj*);
bool isShaderUseUniform4TexSrt(const nn::g3d::MaterialObj*);
bool isShaderUseUniform4ModelUv1(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaBaseColor(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaUniform4(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaUniform4AndVertex(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaVertex(const nn::g3d::MaterialObj*);
bool isShaderUseMulBaseColorVertexColor(const nn::g3d::MaterialObj*);
bool isShaderUseAlphaConstant(const nn::g3d::MaterialObj*);
bool isEnableMaterialLod(const nn::g3d::ModelObj*, s32);
bool isEnableMaterialLod(const nn::g3d::MaterialObj*);
bool isEnableXluZPrePass(const nn::g3d::ModelObj*, s32);
bool isEnableXluZPrePass(const nn::g3d::MaterialObj*);
bool isDisableZPrePass(const nn::g3d::ModelObj*, s32);
bool isDisableZPrePass(const nn::g3d::MaterialObj*);
bool isAlphaMaskUnableZprepass(const nn::g3d::MaterialObj*);
bool isDisableDepthShadow(const nn::g3d::ModelObj*, s32);
bool isDisableDepthShadow(const nn::g3d::MaterialObj*);
bool isDisableStaticDepthShadow(const nn::g3d::ModelObj*, s32);
bool isDisableStaticDepthShadow(const nn::g3d::MaterialObj*);
void getShaderAssignAlphaComponent(const nn::g3d::ModelObj*, s32);
void getShaderAssignAlphaComponent(const nn::g3d::MaterialObj*);
void getShaderAssignAlphaFunc(const nn::g3d::ModelObj*, s32);
void getShaderAssignAlphaFunc(const nn::g3d::MaterialObj*);
void getShaderUseBlendNum(const nn::g3d::MaterialObj*);
void getMaterialDrawPriority(const nn::g3d::ResMaterial*);
void bindShaderParamAndConvertParamCallback(nn::g3d::ResModel*, const al::ShaderHolder*);
void tryActivateLocalShadowMap(agl::DrawContext*, al::ModelAdditionalInfo*, const al::ModelCtrl*);
void getMaxSamplerBias(const nn::g3d::ResMaterial*);
void getMaxSamplerAniso(const nn::g3d::ResMaterial*);
void getMinSamplerMipMax(const nn::g3d::ResMaterial*);
bool isAllSamplerFilterBilinear(const nn::g3d::ResMaterial*);
bool isAllSamplerFilterTrilinear(const nn::g3d::ResMaterial*);
void calcBoundingSphere(const al::ModelCtrl*);
void calcBoundingRadius(const al::ModelCtrl*);
void calcBoundingBox(sead::BoundBox3f*, const al::ModelCtrl*);
void calcBoundingBoxMtx(sead::Matrix34f*, const al::ModelCtrl*);
void calcMeshPolygonNum(const nn::g3d::ResMesh*);
void calcPolygonNum(const nn::g3d::ModelObj*, s32);
void getLodModelCount(const nn::g3d::ModelObj*);
void getLodShapeObjBaseVertex(const nn::g3d::ShapeObj*, s32);
bool isExistLodModel(const nn::g3d::ModelObj*);
bool isShapeVertexPosFormat32bit(const nn::g3d::ShapeObj*);
bool isShapeVertexPosFormat16bit(const nn::g3d::ShapeObj*);
bool isExistMultiIndexVariation(const nn::g3d::ShapeObj*);
bool isIndexFormat8bit(const nn::g3d::ShapeObj*, s32);
bool isIndexFormat16bit(const nn::g3d::ShapeObj*, s32);
bool isIndexFormat32bit(const nn::g3d::ShapeObj*, s32);
bool isIndexTopologyAllTriangleList(const nn::g3d::ShapeObj*);
bool isDisablePrepassCulling(const nn::g3d::ModelObj&);
bool isEnablePrepassCulling(const nn::g3d::ModelObj&, s32);
void tryStoreCreateArgPrepassCulling(al::CulledIndexCreateArg*, const nn::g3d::ModelObj&, s32);
bool isEnableCullPrepass(al::CulledIndexCreateArg*, const nn::g3d::ModelObj&, s32, s32);
}  // namespace alModelFunction
