#pragma once

#include <basis/seadTypes.h>
#include <common/aglRenderBuffer.h>

namespace al {
class EffectSystem;
class LayoutSystem;
class MessageSystem;
class NetworkSystem;
class AudioSystem;
class GamePadSystem;
class HtmlViewer;
class WaveVibrationHolder;
class NfpDirector;
class ApplicationMessageReceiver;
class FontHolder;

struct DrawSystemInfo {
    agl::RenderBuffer* dockedRenderBuffer;
    agl::RenderBuffer* handheldRenderBuffer;
    bool isDocked;
    agl::DrawContext* drawContext;
};

class GameSystemInfo {
public:
    GameSystemInfo();

    AudioSystem* getAudioSystem() const { return mAudioSystem; }

    void setAudioSystem(AudioSystem* audio_system) { mAudioSystem = audio_system; }

    EffectSystem* getEffectSystem() const { return mEffectSystem; }

    void setEffectSystem(EffectSystem* effect_system) { mEffectSystem = effect_system; }

    LayoutSystem* getLayoutSystem() const { return mLayoutSystem; }

    void setLayoutSystem(LayoutSystem* layout_system) { mLayoutSystem = layout_system; }

    MessageSystem* getMessageSystem() const { return mMessageSystem; }

    void setMessageSystem(MessageSystem* message_system) { mMessageSystem = message_system; }

    NetworkSystem* getNetworkSystem() const { return mNetworkSystem; }

    void setNetworkSystem(NetworkSystem* network_system) { mNetworkSystem = network_system; }

    GamePadSystem* getGamePadSystem() const { return mGamePadSystem; }

    void setGamePadSystem(GamePadSystem* game_pad_system) { mGamePadSystem = game_pad_system; }

    DrawSystemInfo* getDrawSystemInfo() const { return mDrawSystemInfo; }

    void setDrawSystemInfo(DrawSystemInfo* draw_system_info) { mDrawSystemInfo = draw_system_info; }

    FontHolder* getFontHolder() const { return mFontHolder; }

    void setFontHolder(FontHolder* font_holder) { mFontHolder = font_holder; }

    NfpDirector* getNfpDirector() const { return mNfpDirector; }

    void setNfpDirector(NfpDirector* nfp_director) { mNfpDirector = nfp_director; }

    HtmlViewer* getHtmlViewer() const { return mHtmlViewer; }

    void setHtmlViewer(HtmlViewer* html_viewer) { mHtmlViewer = html_viewer; }

    ApplicationMessageReceiver* getApplicationMessageReceiver() const {
        return mApplicationMessageReceiver;
    }

    void setApplicationMessageReceiver(ApplicationMessageReceiver* application_message_receiver) {
        mApplicationMessageReceiver = application_message_receiver;
    }

    WaveVibrationHolder* getWaveVibrationHolder() const { return mWaveVibrationHolder; }

    void setWaveVibrationHolder(WaveVibrationHolder* wave_vibration_holder) {
        mWaveVibrationHolder = wave_vibration_holder;
    }

private:
    AudioSystem* mAudioSystem;
    EffectSystem* mEffectSystem;
    LayoutSystem* mLayoutSystem;
    MessageSystem* mMessageSystem;
    NetworkSystem* mNetworkSystem;
    s64 _0x28;
    GamePadSystem* mGamePadSystem;
    DrawSystemInfo* mDrawSystemInfo;
    FontHolder* mFontHolder;
    NfpDirector* mNfpDirector;
    HtmlViewer* mHtmlViewer;
    ApplicationMessageReceiver* mApplicationMessageReceiver;
    WaveVibrationHolder* mWaveVibrationHolder;
};

}  // namespace al
