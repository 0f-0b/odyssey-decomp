#pragma once

#include <basis/seadTypes.h>

namespace al {
class CollisionCodeList;
class CollisionCodeRelationList;
}  // namespace al

namespace alCollisionCodeFunction {
al::CollisionCodeList* tyrCreateCollisionCodeList(const char*);
void tyrGetCollisionRelationList(const al::CollisionCodeList*, const char*);
void getCodeNameByIdx(const al::CollisionCodeList*, s32);
void getDispNameByIdx(const al::CollisionCodeList*, s32);
void getIdxByCodeName(const al::CollisionCodeList*, const char*);
void isExistCodeName(const al::CollisionCodeList*, const char*);
void isValidUserType(const al::CollisionCodeList*, const char*);
void judgeIsBetterCodeName(const al::CollisionCodeList*, const char*, const char*, const char*,
                           const char*);
void judgeIsBetterCodeNameWithAreaName(const al::CollisionCodeRelationList*, const char*,
                                       const char*, const char*, const char*, const char*,
                                       const char*);
}  // namespace alCollisionCodeFunction
