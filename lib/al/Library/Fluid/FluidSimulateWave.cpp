#include "Library/Fluid/FluidSimulateWave.h"

#include <common/aglTextureEnum.h>
#include <common/aglTextureSampler.h>

#include "Library/Draw/GraphicsSystemInfo.h"
#include "Library/Fluid/RippleTextureKeeper.h"
#include "Library/Math/MathUtil.h"
#include "Library/Play/Graphics/NoiseTextureKeeper.h"
#include "Library/Play/Graphics/PointSpriteCursor.h"
#include "Library/Shader/Block/UniformBlock.h"
#include "Library/Shader/Block/UniformBlockUtil.h"
#include "Library/Shader/ForwardRendering/ShaderHolder.h"
#include "Library/Texture/TextureUtil.h"

namespace al {

// 0x1922454
const UniformBlockLayout cLayout0[] = {{0, agl::UniformBlock::Type::_6, 1},
                                       {1, agl::UniformBlock::Type::_6, 1}};
// 0x192246c
const UniformBlockLayout cLayout1[] = {{0, agl::UniformBlock::Type::_6, 1},
                                       {1, agl::UniformBlock::Type::_6, 1}};
// 0x1922478
const UniformBlockLayout cLayout2[] = {{0, agl::UniformBlock::Type::_6, 1}};

FluidSimulateWave::FluidSimulateWave(GraphicsSystemInfo* info, s32 texture_size, f32 _s0, s32 _w3,
                                     bool _w4, bool _w5, const char* _x6)
    : PartsGraphics(info),
      mShaderFluidSimulateWave(info->getShaderHolder()->getShaderProgram("alFluidSimulateWave")),
      mShaderHeightMapToNormal(info->getShaderHolder()->getShaderProgram("alHeightMapToNormal")),
      mShaderPointSprite2D(info->getShaderHolder()->getShaderProgram("alPointSprite2D")),
      mGraphicsSystemInfo(info), mTextureSize(texture_size), _0x330(_s0), _0x3a4(_w3), _0x3ac(_w4),
      _0x3ad(_w5) {
    if (_x6)
        _0x30 = _x6;
    else
        _0x30 = "波紋システム";

    _0x28 = info->getRippleTextureKeeper()->registerRippleTex(this);
    _0x310 = createUniformBlock(cLayout0, 2, nullptr, 2);
    _0x308 = createUniformBlock(cLayout1, 2, nullptr, 2);
    _0x318 = createUniformBlock(cLayout2, 1, nullptr, 2);
    if (_0x3a4 == 1 || _0x3a4 == 2 || _0x3a4 == 5) {
        if (_0x3a4 == 5)
            _0x2a8 = createAglTextureData(agl::TextureFormat::cTextureFormat_R8_G8_B8_A8_sNorm,
                                          mTextureSize, mTextureSize, 1, agl::TextureAttribute::_0);
        else
            _0x2a8 = createAglTextureData(agl::TextureFormat::cTextureFormat_R8_G8_sNorm,
                                          mTextureSize, mTextureSize, 1, agl::TextureAttribute::_0);
    }
    if (_0x3a4 == 3)
        _0x2b0 = createAglTextureData(agl::TextureFormat::cTextureFormat_R16_G16_float,
                                      mTextureSize, mTextureSize, 1, agl::TextureAttribute::_0);
    if (_0x3a4 == 4) {
        _0x2b8 = createAglTextureData(agl::TextureFormat::cTextureFormat_R8_G8_B8_A8_sNorm,
                                      mTextureSize, mTextureSize, 1, agl::TextureAttribute::_0);
        _0x2c0 = createAglTextureData(agl::TextureFormat::cTextureFormat_R8_G8_B8_A8_sNorm,
                                      mTextureSize, mTextureSize, 1, agl::TextureAttribute::_0);
    }

    _0x298 = createAglTextureData(agl::TextureFormat::cTextureFormat_R16_G16_float, mTextureSize,
                                  mTextureSize, 1, agl::TextureAttribute::_0);
    _0x2d8 = new agl::TextureSampler;
    _0x2d8->set_0x154();
    if (_0x3ac) {
        _0x2d8->set_0x157(1);
    } else {
        _0x2d8->set_0x157(5);
        _0x2d8->set_0x138(sead::Color4f::cBlack);
    }
    _0x2d8->applyTextureData(*_0x298);

    _0x2a0 = createAglTextureData(agl::TextureFormat::cTextureFormat_R16_G16_float, mTextureSize,
                                  mTextureSize, 1, agl::TextureAttribute::_0);
    _0x2e0 = new agl::TextureSampler;
    _0x2e0->set_0x154();
    if (_0x3ac) {
        _0x2e0->set_0x157(1);
    } else {
        _0x2e0->set_0x157(5);
        _0x2e0->set_0x138(sead::Color4f::cBlack);
    }
    _0x2e0->applyTextureData(*_0x2a0);

    if (_0x3a4 == 1 || _0x3a4 == 2 || _0x3a4 == 5) {
        _0x2e8 = new agl::TextureSampler;
        _0x2e8->set_0x157(5);
        _0x2e8->set_0x154();
        _0x2e8->applyTextureData(*_0x2a8);
    }
    if (_0x3a4 == 3) {
        _0x2f0 = new agl::TextureSampler;
        _0x2f0->set_0x157(5);
        _0x2f0->set_0x154();
        _0x2f0->applyTextureData(*_0x2b0);
    }
    if (_0x3a4 == 4) {
        _0x2f8 = new agl::TextureSampler;
        _0x2f8->set_0x157(5);
        _0x2f8->set_0x154();
        _0x2f8->applyTextureData(*_0x2b8);

        _0x300 = new agl::TextureSampler;
        _0x300->set_0x157(5);
        _0x300->set_0x154();
        _0x300->applyTextureData(*_0x2c0);
    }

    _0x39c = true;
    _0x39d = true;
    _0x39e = false;
    _0x39f = false;
    _0x3a0 = true;
    _0x3a1 = true;

    _0x354.set(sead::Vector3f::zero);
    _0x360.set(sead::Vector3f::zero);
    makeQuatUpFront(&_0x380, sead::Vector3f::ey, sead::Vector3f::ez);

    _0x170.setDepthTestEnable(false);
    _0x170.setDepthWriteEnable(false);
    _0x170.setBlendEnable(0, true);
    _0x170.getBlendState(0).src_color_factor = sead::Graphics::cBlendFactor_One;
    _0x170.getBlendState(0).dst_color_factor = sead::Graphics::cBlendFactor_One;
    _0x170.getBlendState(0).color_equation = sead::Graphics::cBlendEquation_Add;
    _0x170.set_0xe8(false);
    _0x170.setColorMask(0x000000ff);

    _0x148 =
        new PointSpriteCursor("ポイントスプライト", 300, nullptr, PointSpriteCursor::DrawType::_4);
    _0x150 = new PointSpriteCursor("矩形スプライト", 300, nullptr, PointSpriteCursor::DrawType::_5);
    _0x148->beginStore();
    _0x150->beginStore();

    if (!_0x3e4)
        return;

    switch (_0x3e8) {
    case 0:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingCaustics3DTexture();
        break;
    case 1:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingCloudLikeFbm3DNoiseTexture();
        break;
    case 2:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingCurlNoise3DTexture();
        break;
    case 3:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingFrostNoise3DTexture();
        break;
    case 4:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingGemNoise3DTexture();
        break;
    case 5:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingOceanFoam3DNoiseTexture();
        break;
    case 6:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingPerlinNoise3DTexture();
        break;
    case 7:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingPerlinFbmNoise3DTexture();
        break;
    case 8:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingRidgeNoise3DTexture();
        break;
    case 9:
        mGraphicsSystemInfo->getNoiseTextureKeeper()->declareUsingWorleyNoise3DTexture();
        break;
    }
}

void FluidSimulateWave::beginStore() {
    _0x148->beginStore();
    _0x150->beginStore();
}

void FluidSimulateWave::initGaussian(s32 _w1) {
    _0x3b8 = _w1;
    _0x3b4 = true;
    _0x2c8 = createAglTextureData(agl::TextureFormat::cTextureFormat_R16_G16_float, mTextureSize,
                                  mTextureSize, 1, agl::TextureAttribute::_0);
    _0x2d0 = createAglTextureData(agl::TextureFormat::cTextureFormat_R16_G16_float, mTextureSize,
                                  mTextureSize, 1, agl::TextureAttribute::_0);
}

void FluidSimulateWave::finalize() {
    if (_0x310) {
        delete _0x310;
        _0x310 = nullptr;
    }
    if (_0x308) {
        delete _0x308;
        _0x308 = nullptr;
    }
    if (_0x318) {
        delete _0x318;
        _0x318 = nullptr;
    }

    destroyAglTextureAndImage(&_0x298);
    destroyAglTextureAndImage(&_0x2a0);
    destroyAglTextureAndImage(&_0x2a8);
    destroyAglTextureAndImage(&_0x2b0);
    destroyAglTextureAndImage(&_0x2b8);
    destroyAglTextureAndImage(&_0x2c0);

    if (_0x148) {
        delete _0x148;
        _0x148 = nullptr;
    }
    if (_0x150) {
        delete _0x150;
        _0x150 = nullptr;
    }

    if (_0x2d8) {
        delete _0x2d8;
        _0x2d8 = nullptr;
    }
    if (_0x2e0) {
        delete _0x2e0;
        _0x2e0 = nullptr;
    }
    if (_0x2e8) {
        delete _0x2e8;
        _0x2e8 = nullptr;
    }
    if (_0x2f0) {
        delete _0x2f0;
        _0x2f0 = nullptr;
    }
    if (_0x2f8) {
        delete _0x2f8;
        _0x2f8 = nullptr;
    }
    if (_0x300) {
        delete _0x300;
        _0x300 = nullptr;
    }

    if (_0x2c8) {
        delete _0x2c8;
        _0x2c8 = nullptr;
    }
    if (_0x2d0) {
        delete _0x2d0;
        _0x2d0 = nullptr;
    }
}

struct GraphicsUpdateInfo {
    f32 _0x0;
};

void FluidSimulateWave::update(const GraphicsUpdateInfo& info) {
    _0x33c = _0x338;
    _0x338 += info._0x0;
    (&_0x298)[_0x328]->invalidateCPUCache();
    if (_0x3a4 == 1 || _0x3a4 == 2 || _0x3a4 == 5)
        _0x2a8->invalidateCPUCache();
}

void FluidSimulateWave::calcGpu(const GraphicsCalcGpuInfo& info) {
    {
        UniformBlockSetter setter{_0x310, 0};
        _0x310->setV4f(0, {1.0f / mTextureSize, 1.0f / mTextureSize, _0x330 * _0x334 / mTextureSize,
                           _0x330 * _0x334 / mTextureSize});
        if (_0x3e4)
            _0x310->setV4f(1, {_0x3cc.x, _0x3cc.y, _0x3cc.z, _0x3bc});
    }
    _0x39c = std::exchange(_0x39d, false);
    _0x3a0 = std::exchange(_0x3a1, false);
    f32 _s8 = _0x338 - _0x33c;
    if (isNearZero(_s8)) {
        _0x33c = _0x338;
        {
            UniformBlockSetter setter{_0x308, 0};
            _0x308->setV4f(0, {1.0f / mTextureSize, 1.0f / mTextureSize, 0, 0});
        }
        _0x3a0 = false;
    } else {
        if (_0x3a0) {
            if (_0x3e4)
                _0x3cc += _0x3c0;
            _0x328 = !_0x328;

            f32 _v1 = _0x344;
            f32 _v2 = _0x330 * _0x334 / mTextureSize;
            f32 _v3 = _0x340 * _0x340;
            f32 _v4 = _v3 * _s8 * _s8;
            f32 _v5 = _v4 / (_v2 * _v2);
            f32 _v6 = _s8 * _v1;
            f32 _v7 = _v5 * -8.0f + 4.0f;
            f32 _v8 = _v6 + 2.0f;
            f32 _v9 = _v7 / _v8;
            f32 _v10 = _v6 + -2.0f;
            f32 _v11 = _v10 / _v8;
            f32 _v12 = _v5 * 2.0f;
            f32 _v13 = _v12 / _v8;

            sead::Quatf _sp_0x40;
            makeQuatFrontUp(&_sp_0x40, sead::Vector3f::ez, sead::Vector3f::ey);
            sead::Quatf _sp_0x30;
            makeQuatFromToQuat(&_sp_0x30, _0x380, _sp_0x40);
            {
                UniformBlockSetter setter{_0x308, 0};
                _0x308->setV4f(0, {1.0f / mTextureSize, 1.0f / mTextureSize,
                                   _0x36c.x / mTextureSize + _0x34c,
                                   _0x36c.y / mTextureSize + _0x350});
                _0x308->setV4f(1, {_v9, _v11, _v13, _0x348});
                if (_0x3e4) {
                    _0x3d8 = {_0x3bc * _0x36c.x / mTextureSize, 0,
                              _0x3bc * _0x36c.y / mTextureSize};
                    _0x3cc += _0x3d8;
                    UniformBlockSetter setter{_0x310, 0};
                    _0x310->setV4f(0, {1.0f / mTextureSize, 1.0f / mTextureSize,
                                       _0x330 * _0x334 / mTextureSize,
                                       _0x330 * _0x334 / mTextureSize});
                    if (_0x3e4)
                        _0x310->setV4f(1, {_0x3cc.x, _0x3cc.y, _0x3cc.z, _0x3bc});
                }
            }
            _0x36c.set(sead::Vector2f::zero);
            {
                UniformBlockSetter setter{_0x318, 0};
                sead::Vector3f _sp = _0x374;
                _sp -= _0x390;
                rotateVectorQuat(&_sp, _sp_0x30);
                _sp += _0x390;
                _0x318->setV4f(0, {_sp.x, _sp.z, _0x330 * _0x334 / mTextureSize,
                                   _0x330 * _0x334 / mTextureSize});
            }
            if (_0x39f) {
                _0x148->endStore();
                _0x150->endStore();
            }
        }
        _0x39e = std::exchange(_0x39f, false);
        _0x33c = _0x338;
    }
}

void FluidSimulateWave::endStore() {
    _0x148->endStore();
    _0x150->endStore();
}

}  // namespace al
