#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

namespace ultradope {

Color SkyColorForTime(const GameSession& session);
void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime, bool moving);

}  // namespace ultradope
