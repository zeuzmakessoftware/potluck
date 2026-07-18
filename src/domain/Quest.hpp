#pragma once

#include "domain/Types.hpp"

namespace ultradope {

enum class QuestEvent {
    PlotTilled,
    CropPlanted,
    PlotWatered,
    CropHarvested,
    ItemShipped,
    SaleCompleted,
    SpokeToTessa,
    HydroBuildValidated
};

struct QuestProgress {
    QuestId active = QuestId::OutdoorBasics;
    int tilled = 0;
    int planted = 0;
    int watered = 0;
    int harvested = 0;
    int shipped = 0;
    int salesCompleted = 0;
    bool spokeToTessa = false;
    bool hydroBuildValidated = false;

};

const char* QuestTitle(QuestId quest);

}  // namespace ultradope
