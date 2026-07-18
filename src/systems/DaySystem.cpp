#include "systems/DaySystem.hpp"

#include "systems/EconomySystem.hpp"

#include <algorithm>

namespace ultradope {

bool AdvanceClock(GameSession& session, float realSeconds) {
    if (session.progression.environment == EnvironmentId::HydroLab) return false;
    session.calendar.minuteOfDay += std::max(0.0F, realSeconds);
    return session.calendar.minuteOfDay >= 1320.0F;
}

Weather WeatherForDay(std::uint32_t worldSeed, int day) {
    std::uint32_t value = worldSeed ^ (static_cast<std::uint32_t>(day) * 0x9E3779B9U);
    value ^= value >> 16U;
    value *= 0x7FEB352DU;
    value ^= value >> 15U;
    return value % 5U == 0U ? Weather::Rainy : Weather::Sunny;
}

DaySummary EndDay(GameSession& session) {
    DaySummary summary = SettleShipment(session);
    const bool rain = session.calendar.weather == Weather::Rainy;
    for (auto& plot : session.farm.plots) {
        if (plot.crop == CropType::None || plot.withered) {
            plot.wateredToday = false;
            continue;
        }
        if (plot.wateredToday || rain) {
            if (!IsMature(plot)) ++plot.growthDays;
            plot.neglectDays = 0;
        } else {
            ++plot.neglectDays;
            if (plot.neglectDays > GetCropDefinition(plot.crop).neglectTolerance) {
                plot.withered = true;
            }
        }
        plot.wateredToday = false;
    }

    ++session.calendar.day;
    session.calendar.minuteOfDay = 360.0F;
    session.calendar.weather = WeatherForDay(session.worldSeed, session.calendar.day);
    session.player.energy = session.player.maxEnergy;
    session.player.position = {0.0F, 9.0F};
    session.farm.wateringCan = session.farm.wateringCanCapacity;
    session.lastSummary = summary;
    return summary;
}

}  // namespace ultradope
