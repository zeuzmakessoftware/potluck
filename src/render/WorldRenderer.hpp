#pragma once

#include "domain/GameSession.hpp"
#include "render/CharacterRenderer.hpp"

#include <raylib.h>

namespace ultradope {

bool LoadWorldRendererAssets();
void UnloadWorldRendererAssets();
Color SkyColorForTime(const GameSession& session);
void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime,
                      const PlayerRenderState& player, float doorOpenFraction);
void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime, bool moving);

}  // namespace ultradope
