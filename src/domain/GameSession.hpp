#pragma once

#include "domain/Farm.hpp"
#include "domain/Hydroponics.hpp"
#include "domain/Inventory.hpp"
#include "domain/Quest.hpp"
#include "domain/Progression.hpp"

#include <array>
#include <string>

namespace ultradope {

struct PlayerState {
    Vec2 position{0.0F, 9.0F};
    float facingRadians = 3.14159F;
    int energy = 100;
    int maxEnergy = 100;
};

struct CalendarState {
    int day = 1;
    float minuteOfDay = 360.0F;
    Weather weather = Weather::Sunny;
};

struct ShipmentState {
    std::array<int, 3> cropCounts{};
};

struct DaySummary {
    int dayEnded = 0;
    int itemsSold = 0;
    int revenue = 0;
    std::string note;
};

struct GameSession {
    PlayerState player;
    CalendarState calendar;
    FarmState farm;
    Inventory inventory;
    ShipmentState shipment;
    QuestProgress quests;
    ProgressionState progression;
    HydroState hydro;
    Tool selectedTool = Tool::Hoe;
    CropType selectedCrop = CropType::Cinderleaf;
    int money = 250;
    int reputation = 0;
    int lifetimeRevenue = 0;
    std::uint32_t worldSeed = 0x554C5452U;
    bool violetUnlocked = false;
    bool sunspunUnlocked = false;
    DaySummary lastSummary;

    static GameSession NewGame();
};

const char* WeekdayName(int day);
std::string FormatTime(float minuteOfDay);

}  // namespace ultradope
