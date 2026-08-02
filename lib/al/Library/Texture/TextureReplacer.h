#pragma once

namespace agl {
class TextureData;
}

namespace al {
class LiveActor;

class TextureReplacer {
public:
    TextureReplacer();
    TextureReplacer(const agl::TextureData* textureData);

    void setup(const agl::TextureData* textureData);
    void replace(LiveActor* actor, const char* sourceTextureName, const char* replaceTextureName);
    void update();

private:
    void* _padding[0x4];
};

static_assert(sizeof(TextureReplacer) == 0x20);

}  // namespace al
