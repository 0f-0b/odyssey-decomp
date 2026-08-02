#include "System/CollectBgmBailoutInfo.h"

CollectBgmBailoutInfo::CollectBgmBailoutInfo()
    : CollectBgmBailoutInfo(nullptr, nullptr, nullptr, nullptr) {}

CollectBgmBailoutInfo::CollectBgmBailoutInfo(const char* name1, const char* situation_name1,
                                             const char* name2, const char* situation_name2)
    : name1(name1), situationName1(situation_name1), name2(name2), situationName2(situation_name2) {
}

const s32 cCollectBgmBailoutInfoTableSize = 20;
CollectBgmBailoutInfo cCollectBgmBailoutInfoTable[cCollectBgmBailoutInfoTableSize] = {
    {"StmRsBgmDesertNight", nullptr, "StmRsBgmDesertNight", "In2DArea"},
    {"StmRsBgmBossHaikai_B", nullptr, "StmRsBgmBossHaikai_B", "CollectBgmGiantWanderBoss"},
    {"StmRsBgmBossHaikai_B", nullptr, "StmRsBgmBossHaikai_B", "CollectBgmGiantWanderBoss8Bit"},
    {"StmRsBgmDemoWorldMap", nullptr, "StmRsBgmDemoWorldMap", "CollectBgmWorldMap"},
    {"StmRsBgmCityScenario03JpRadio", nullptr, "StmRsBgmCityScenario03Jp", nullptr},
    {"StmRsBgmEndRock", nullptr, "StmRsBgmEndRockClct", nullptr},
    {"StmRsBgmEndRockJpRadio", nullptr, "StmRsBgmEndRockJp", nullptr},
    {"StmRsBgmDesertTown", nullptr, "StmRsBgmDesertTownClct", nullptr},
    {"StmRsBgmCityScenario03", nullptr, "StmRsBgmCityScenario03Clct", nullptr},
    {"StmRsBgmCitySession", "PlayEtcInst", "StmRsBgmCitySessionCheck", nullptr},
    {"StmRsBgmShop01", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop02", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop03", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop04", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop01Radio", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop02Radio", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop03Radio", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmShop04Radio", nullptr, "StmRsBgmShopClct", nullptr},
    {"StmRsBgmLakeScenario2", "In2DArea", "StmRsBgmLakeScenario1", "In2DArea"},
    {"StmRsBgmCityCafe01Radio", nullptr, "StmRsBgmCityCafe01", nullptr},
};
