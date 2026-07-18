#include "domain/Farm.hpp"

#include <algorithm>

namespace ultradope {
namespace {
constexpr CropDefinition kEmpty{};
constexpr CropDefinition kCinder{CropType::Cinderleaf, "Cinderleaf", 3, 0, 30, 58};
constexpr CropDefinition kViolet{CropType::VioletComet, "Violet Comet", 5, 0, 55, 125};
constexpr CropDefinition kSunspun{CropType::Sunspun, "Sunspun", 7, 1, 90, 225};
}  // namespace

const CropDefinition& GetCropDefinition(CropType crop) {
    switch (crop) {
        case CropType::Cinderleaf: return kCinder;
        case CropType::VioletComet: return kViolet;
        case CropType::Sunspun: return kSunspun;
        case CropType::None: return kEmpty;
    }
    return kEmpty;
}

bool IsMature(const SoilPlot& plot) {
    return plot.crop != CropType::None && !plot.withered &&
           plot.growthDays >= GetCropDefinition(plot.crop).growthDays;
}

int VisualGrowthStage(const SoilPlot& plot) {
    if (plot.crop == CropType::None) return 0;
    if (plot.withered) return 6;
    const int required = std::max(1, GetCropDefinition(plot.crop).growthDays);
    return std::clamp(1 + (plot.growthDays * 4) / required, 1, 5);
}

FarmState CreateFarm() {
    FarmState farm;
    for (std::size_t index = 0; index < farm.plots.size(); ++index) {
        farm.plots[index].row = static_cast<int>(index / 6U);
        farm.plots[index].column = static_cast<int>(index % 6U);
    }
    return farm;
}

const char* CropName(CropType crop) { return GetCropDefinition(crop).name; }

const char* ItemName(ItemType item) {
    switch (item) {
        case ItemType::CinderleafSeed: return "Cinderleaf Seeds";
        case ItemType::VioletCometSeed: return "Violet Comet Seeds";
        case ItemType::SunspunSeed: return "Sunspun Seeds";
        case ItemType::CinderleafFlower: return "Cinderleaf Flower";
        case ItemType::VioletCometFlower: return "Violet Comet Flower";
        case ItemType::SunspunFlower: return "Sunspun Flower";
        case ItemType::None: return "Empty";
    }
    return "Unknown";
}

ItemType SeedItem(CropType crop) {
    switch (crop) {
        case CropType::Cinderleaf: return ItemType::CinderleafSeed;
        case CropType::VioletComet: return ItemType::VioletCometSeed;
        case CropType::Sunspun: return ItemType::SunspunSeed;
        case CropType::None: return ItemType::None;
    }
    return ItemType::None;
}

ItemType HarvestItem(CropType crop) {
    switch (crop) {
        case CropType::Cinderleaf: return ItemType::CinderleafFlower;
        case CropType::VioletComet: return ItemType::VioletCometFlower;
        case CropType::Sunspun: return ItemType::SunspunFlower;
        case CropType::None: return ItemType::None;
    }
    return ItemType::None;
}

CropType CropFromSeed(ItemType item) {
    switch (item) {
        case ItemType::CinderleafSeed: return CropType::Cinderleaf;
        case ItemType::VioletCometSeed: return CropType::VioletComet;
        case ItemType::SunspunSeed: return CropType::Sunspun;
        default: return CropType::None;
    }
}

}  // namespace ultradope
