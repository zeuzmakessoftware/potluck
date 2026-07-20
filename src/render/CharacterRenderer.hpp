#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

namespace ultradope {

struct PlayerRenderState {
    Vec2 position{};
    float facingRadians = 0.0F;
    float animationTime = 0.0F;
    float walkBlend = 0.0F;
    float doorReachBlend = 0.0F;
};

void DrawPlayerCharacter(const PlayerRenderState& state);
void DrawPlayerCharacter(const PlayerState& player, float animationTime, bool moving);
void DrawNpcCharacter(Vector3 position, Color clothing, Color accent, const char* name,
                      float animationTime);

}  // namespace ultradope
