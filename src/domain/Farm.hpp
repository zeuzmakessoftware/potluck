#pragma once

#include "domain/Types.hpp"

#include <array>

namespace ultradope {

struct CropDefinition {
    CropType type = CropType::None;
    const char* name = "Empty";
    int growthDays = 0;
    int neglectTolerance = 0;
    int seedPrice = 0;
    int salePrice = 0;
};

struct SoilPlot {
    int row = 0;
    int column = 0;
    bool tilled = false;
    CropType crop = CropType::None;
    int growthDays = 0;
    int neglectDays = 0;
    bool wateredToday = false;
    bool withered = false;
};

struct FarmState {
    static constexpr std::size_t kPlotCount = 24;
    std::array<SoilPlot, kPlotCount> plots{};
    int wateringCan = 12;
    int wateringCanCapacity = 12;
};

const CropDefinition& GetCropDefinition(CropType crop);
bool IsMature(const SoilPlot& plot);
int VisualGrowthStage(const SoilPlot& plot);
FarmState CreateFarm();

}  // namespace ultradope
