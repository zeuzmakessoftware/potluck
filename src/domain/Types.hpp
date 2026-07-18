#pragma once

#include <cstdint>

namespace ultradope {

struct Vec2 {
    float x = 0.0F;
    float y = 0.0F;
};

enum class CropType : std::uint8_t { None, Cinderleaf, VioletComet, Sunspun };
enum class ItemType : std::uint8_t {
    None,
    CinderleafSeed,
    VioletCometSeed,
    SunspunSeed,
    CinderleafFlower,
    VioletCometFlower,
    SunspunFlower
};
enum class Tool : std::uint8_t { Hoe, WateringCan, Hand, Seeds };
enum class Weather : std::uint8_t { Sunny, Rainy };
enum class QuestId : std::uint8_t {
    OutdoorBasics,
    FirstShipment,
    HydroInvitation,
    HydroAssembly,
    Complete
};

const char* CropName(CropType crop);
const char* ItemName(ItemType item);
ItemType SeedItem(CropType crop);
ItemType HarvestItem(CropType crop);
CropType CropFromSeed(ItemType item);

}  // namespace ultradope
