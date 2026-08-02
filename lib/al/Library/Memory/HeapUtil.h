#pragma once

#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

namespace al {
class AudioResourceDirector;

sead::Heap* getStationedHeap();
sead::Heap* getSequenceHeap();
sead::Heap* getSceneResourceHeap();
sead::Heap* getSceneHeap();
sead::Heap* getCourseSelectResourceHeap();
sead::Heap* getCourseSelectHeap();
sead::Heap* getWorldResourceHeap();
sead::Heap* tryFindNamedHeap(const char* heapName);
sead::Heap* findNamedHeap(const char* heapName);
void addNamedHeap(sead::Heap* heap, const char* heapName);
void removeNamedHeap(const char* heapName);
void createSequenceHeap();
void freeAllSequenceHeap();
void printAllSequenceHeap();
void createSceneHeap(const char* stageName, bool backwards);
void createSceneResourceHeap(const char* stageName);
bool isCreatedSceneResourceHeap();
void destroySceneHeap(bool removeCategory);
void createCourseSelectHeap();
void destroyCourseSelectHeap();
void createWorldResourceHeap(bool useCategory);
void destroyWorldResourceHeap(bool removeCategory);
void loadPlayerResource(const char* categoryName);
void freePlayerResource(const char* categoryName);
void setAudioResourceDirectorToMemorySystem(AudioResourceDirector* audioResourceDirector);

class SceneHeapSetter : public sead::ScopedCurrentHeapSetter {
public:
    SceneHeapSetter();

private:
    sead::Heap* mHeap = getSceneHeap();
};

static_assert(sizeof(SceneHeapSetter) == 0x10);

void copyMemoryFast(u32* dest, const u32* src, u32 nbytes);
void copyMemory(void* dest, const void* src, u32 nbytes);
bool tryCompressByZlib(u8*, u32*, const u8*, u32);
bool tryDecompressByZlib(u8*, u32*, const u8*, u32);
sead::Heap* getCurrentHeap();
}  // namespace al
