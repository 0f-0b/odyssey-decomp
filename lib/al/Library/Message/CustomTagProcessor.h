#pragma once

#include <basis/seadTypes.h>
#include <nn/font/font_Util.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Message/IUseMessageSystem.h"

namespace nn::font {
template <typename T>
class PrintContext;
class Rectangle;
template <typename T>
class TextWriterBase;

template <typename T>
class TagProcessorBase {
public:
    NN_RUNTIME_TYPEINFO_BASE()

    virtual ~TagProcessorBase();
    virtual void Process(u32, nn::font::PrintContext<T>*);
    virtual void CalculateRect(nn::font::Rectangle*, nn::font::PrintContext<T>*, u32);
    virtual void BeginPrint(nn::font::PrintContext<T>*);
    virtual void EndPrint(nn::font::PrintContext<T>*);
    virtual void BeginCalculateRect(nn::font::PrintContext<T>*);
    virtual void EndCalculateRect(nn::font::PrintContext<T>*);
};

}  // namespace nn::font

namespace sead {

template <typename T>
class MessageSet {
public:
    class TagInfo;
};

}  // namespace sead

namespace eui {
class TextBoxEx;

class TagProcessor : public nn::font::TagProcessorBase<u16> {
public:
    virtual void Process(u32, nn::font::PrintContext<u16>*) override;
    virtual void CalculateRect(nn::font::Rectangle*, nn::font::PrintContext<u16>*, u32) override;
    virtual void BeginPrint(nn::font::PrintContext<u16>*) override;
    virtual void EndPrint(nn::font::PrintContext<u16>*) override;
    virtual void BeginCalculateRect(nn::font::PrintContext<u16>*) override;
    virtual void EndCalculateRect(nn::font::PrintContext<u16>*) override;
    virtual void preProcess(char16*, u32*, u32*, u32, const char16*, u32, s32, u32, void*);
    virtual void calcAdjustTextScale(TextBoxEx*, f32, f32) const;

protected:
    virtual void preProcessSystemTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*, u32*,
                                      u32, const char16*, u32, void*);
    virtual void preProcessEuiTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*, u32*,
                                   u32, const char16*, u32, void*);
    virtual void preProcessEuiPictFontTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*,
                                           u32*, u32, const char16*, u32, void*);
    virtual void preProcessGrammarTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*,
                                       u32*, u32, const char16*, u32, void*);
    virtual void preProcessAppTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*, u32*,
                                   u32, const char16*, u32, void*);
    virtual void process_(u16, nn::font::PrintContext<u16>*, nn::font::Rectangle*);
    virtual void processEuiTag_(const sead::MessageSet<char16>::TagInfo*,
                                nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void processAppTag_(const sead::MessageSet<char16>::TagInfo*,
                                nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void processRubyTag_(const sead::MessageSet<char16>::TagInfo*,
                                 nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void adjustRubyCursorY(nn::font::TextWriterBase<u16>*,
                                   const sead::MessageSet<char16>::TagInfo*,
                                   nn::font::PrintContext<u16>*);
    virtual void processFontTag_(const sead::MessageSet<char16>::TagInfo*,
                                 nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void processSizeTag_(const sead::MessageSet<char16>::TagInfo*,
                                 nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void processColorTag_(const sead::MessageSet<char16>::TagInfo*,
                                  nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                  const char16*);
    virtual void processPageBreakTag_(const sead::MessageSet<char16>::TagInfo*,
                                      nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                      const char16*);
    virtual void processAlphaTag_(const sead::MessageSet<char16>::TagInfo*,
                                  nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                  const char16*);
    virtual void processPictFontProcessTag_(const sead::MessageSet<char16>::TagInfo*,
                                            nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                            const char16*);
    virtual void processSkipTag_(const sead::MessageSet<char16>::TagInfo*,
                                 nn::font::PrintContext<u16>*, nn::font::Rectangle*, const char16*);
    virtual void processFitWidthTag_(const sead::MessageSet<char16>::TagInfo*,
                                     nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                     const char16*);
    virtual void getRubyScale_() const;
    virtual void getRubyScaleMax_() const;
    virtual void getRubyBaseLinkOffset_() const;
    virtual void getRubyCharSpace_() const;
    virtual void getPictFontScale_() const;
    virtual void getPictFontCodeAndFont_(char16*, u16*, u8) const;
};

}  // namespace eui

namespace al {

class CustomTagProcessor : public eui::TagProcessor, public HioNode, public IUseMessageSystem {
public:
    // TODO this function should be inline
    const MessageSystem* getMessageSystem() const override;

protected:
    void preProcessAppTag_(const sead::MessageSet<char16>::TagInfo*, char16*, u32*, u32*, u32,
                           const char16*, u32, void*) override;
    void processAppTag_(const sead::MessageSet<char16>::TagInfo*, nn::font::PrintContext<u16>*,
                        nn::font::Rectangle*, const char16*) override;
    void processPictFontProcessTag_(const sead::MessageSet<char16>::TagInfo*,
                                    nn::font::PrintContext<u16>*, nn::font::Rectangle*,
                                    const char16*) override;

    // TODO these too
    void getRubyScale_() const override;
    void getRubyBaseLinkOffset_() const override;
    void getRubyCharSpace_() const override;
    void getPictFontScale_() const override;
};

}  // namespace al
