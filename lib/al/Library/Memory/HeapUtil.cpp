#include "Library/Memory/HeapUtil.h"

#include <heap/seadHeapMgr.h>

#include "Library/File/FileUtil.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/System/SystemKit.h"
#include "Project/Memory/MemorySystem.h"

#include "System/ProjectInterface.h"

namespace al {
sead::Heap* getStationedHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getStationedHeap();
}

sead::Heap* getSequenceHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getSequenceHeap();
}

sead::Heap* getSceneResourceHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getSceneResourceHeap();
}

sead::Heap* getSceneHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getSceneHeap();
}

sead::Heap* getCourseSelectResourceHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getCourseSelectResourceHeap();
}

sead::Heap* getCourseSelectHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getCourseSelectHeap();
}

sead::Heap* getWorldResourceHeap() {
    return alProjectInterface::getSystemKit()->getMemorySystem()->getWorldResourceHeap();
}

sead::Heap* tryFindNamedHeap(const char* heapName) {
    return alProjectInterface::getSystemKit()->getMemorySystem()->tryFindNamedHeap(heapName);
}

sead::Heap* findNamedHeap(const char* heapName) {
    return alProjectInterface::getSystemKit()->getMemorySystem()->findNamedHeap(heapName);
}

void addNamedHeap(sead::Heap* heap, const char* heapName) {
    alProjectInterface::getSystemKit()->getMemorySystem()->addNamedHeap(heap, heapName);
}

void removeNamedHeap(const char* heapName) {
    alProjectInterface::getSystemKit()->getMemorySystem()->removeNamedHeap(heapName);
}

void createSequenceHeap() {
    alProjectInterface::getSystemKit()->getMemorySystem()->createSequenceHeap();

    addResourceCategory("シーケンス", 0x18, getSequenceHeap());
    setCurrentCategoryName("シーケンス");
    clearFileLoaderEntry();
}

void freeAllSequenceHeap() {
    removeResourceCategory("シーケンス");
    alProjectInterface::getSystemKit()->getMemorySystem()->freeAllSequenceHeap();

    addResourceCategory("シーケンス", 0x18, getSequenceHeap());
    setCurrentCategoryName("シーケンス");
    clearFileLoaderEntry();
}

void printAllSequenceHeap() {
    alProjectInterface::getSystemKit()->getMemorySystem()->printSequenceHeap();
}

void createSceneHeap(const char* stageName, bool backwards) {
    sead::ScopedCurrentHeapSetter heapSetter{getSequenceHeap()};

    SystemKit* systemKit = alProjectInterface::getSystemKit();
    bool isSceneHeapCreated = systemKit->getMemorySystem()->createSceneHeap(stageName, backwards);
    if (isSceneHeapCreated) {
        addResourceCategory("シーン", 0x200, getSceneResourceHeap());
        addResourceCategory("シーン", 0x200, getSceneResourceHeap());
        setCurrentCategoryName("シーン");
        clearFileLoaderEntry();
    }
}

void createSceneResourceHeap(const char* stageName) {
    sead::ScopedCurrentHeapSetter heapSetter{getSequenceHeap()};

    alProjectInterface::getSystemKit()->getMemorySystem()->createSceneResourceHeap(stageName,
                                                                                   false);

    addResourceCategory("シーン", 0x200, getSceneResourceHeap());
    addResourceCategory("シーン", 0x200, getSceneResourceHeap());
    setCurrentCategoryName("シーン");
    clearFileLoaderEntry();
}

bool isCreatedSceneResourceHeap() {
    return getSceneResourceHeap() != nullptr;
}

void destroySceneHeap(bool removeCategory) {
    if (removeCategory) {
        removeResourceCategory("シーン");
        removeResourceCategory("シーン[デバッグ]");
        alProjectInterface::getSystemKit()->getMemorySystem()->destroySceneHeap();
        alProjectInterface::getSystemKit()->getMemorySystem()->destroySceneResourceHeap();

        return;
    }

    alProjectInterface::getSystemKit()->getMemorySystem()->destroySceneHeap();
}

void createCourseSelectHeap() {
    sead::ScopedCurrentHeapSetter heapSetter{getSequenceHeap()};

    alProjectInterface::getSystemKit()->getMemorySystem()->createCourseSelectHeap();

    addResourceCategory("コースセレクト", 0x40, getCourseSelectResourceHeap());
    setCurrentCategoryName("コースセレクト");
    clearFileLoaderEntry();
}

void destroyCourseSelectHeap() {
    removeResourceCategory("コースセレクト");
    alProjectInterface::getSystemKit()->getMemorySystem()->destroyCourseSelectHeap();
}

void createWorldResourceHeap(bool useCategory) {
    sead::ScopedCurrentHeapSetter heapSetter{getSequenceHeap()};

    alProjectInterface::getSystemKit()->getMemorySystem()->createWorldResourceHeap();

    if (useCategory) {
        addResourceCategory("ワールド常駐", 0x400, getWorldResourceHeap());
        setCurrentCategoryName("ワールド常駐");
    }
    clearFileLoaderEntry();
}

void destroyWorldResourceHeap(bool removeCategory) {
    if (removeCategory)
        removeResourceCategory("ワールド常駐");

    clearFileLoaderEntry();
    alProjectInterface::getSystemKit()->getMemorySystem()->destroyWorldResourceHeap();
}

void loadPlayerResource(const char* categoryName) {
    addResourceCategory(
        categoryName, 0x14,
        alProjectInterface::getSystemKit()->getMemorySystem()->getPlayerResourceHeap());
    createCategoryResourceAll(categoryName);
}

void freePlayerResource(const char* categoryName) {
    removeResourceCategory(categoryName);
    alProjectInterface::getSystemKit()->getMemorySystem()->freeAllPlayerHeap();
}

void setAudioResourceDirectorToMemorySystem(AudioResourceDirector* audioResourceDirector) {
    alProjectInterface::getSystemKit()->getMemorySystem()->setAudioResourceDirector(
        audioResourceDirector);
}

SceneHeapSetter::SceneHeapSetter() : sead::ScopedCurrentHeapSetter(getSceneHeap()) {}

void copyMemoryFast(u32* dest, const u32* src, u32 nbytes) {
    u32 nblocks = nbytes / 16;
    while (nblocks--) {
        u32 a = *src++;
        u32 b = *src++;
        u32 c = *src++;
        u32 d = *src++;
        *dest++ = a;
        *dest++ = b;
        *dest++ = c;
        *dest++ = d;
    }
}

void copyMemory(void* dest, const void* src, u32 nbytes) {
    u32 src_offset = reinterpret_cast<uintptr_t>(src) % 4;
    u32 dest_offset = reinterpret_cast<uintptr_t>(dest) % 4;
    if (nbytes % 16 == 0 && src_offset == dest_offset) {
        copyMemoryFast(reinterpret_cast<u32*>(dest), reinterpret_cast<const u32*>(src), nbytes);
        return;
    }

    const u8* src_u8 = reinterpret_cast<const u8*>(src);
    u8* dest_u8 = reinterpret_cast<u8*>(dest);
    if (nbytes >= 16 && src_offset == dest_offset) {
        if (src_offset != 0) {
            for (u8 i = 4 - src_offset; i--;) {
                *dest_u8++ = *src_u8++;
                nbytes--;
            }
        }

        u32* dest_u32 = reinterpret_cast<u32*>(dest_u8);
        const u32* src_u32 = reinterpret_cast<const u32*>(src_u8);
        while (nbytes >= 4) {
            *dest_u32++ = *src_u32++;
            nbytes -= 4;
        }

        src_u8 = reinterpret_cast<const u8*>(src_u32);
        dest_u8 = reinterpret_cast<u8*>(dest_u32);
        while (nbytes--)
            *dest_u8++ = *src_u8++;
        return;
    }

    while (nbytes--)
        *dest_u8++ = *src_u8++;
}

bool tryCompressByZlib(u8*, u32*, const u8*, u32) {
    return false;
}

bool tryDecompressByZlib(u8*, u32*, const u8*, u32) {
    return false;
}

sead::Heap* getCurrentHeap() {
    return sead::HeapMgr::instance()->getCurrentHeap();
}

}  // namespace al
