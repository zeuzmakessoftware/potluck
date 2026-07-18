#pragma once

#include "domain/GameSession.hpp"

namespace ultradope {

bool AdvanceClock(GameSession& session, float realSeconds);
Weather WeatherForDay(std::uint32_t worldSeed, int day);
DaySummary EndDay(GameSession& session);

}  // namespace ultradope
