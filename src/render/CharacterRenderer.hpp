#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

namespace ultradope {

void DrawPlayerCharacter(const PlayerState& player, float animationTime, bool moving);
void DrawNpcCharacter(Vector3 position, Color clothing, Color accent, const char* name,
                      float animationTime);

}  // namespace ultradope
