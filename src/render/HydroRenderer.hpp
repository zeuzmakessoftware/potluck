#pragma once

#include "domain/GameSession.hpp"

namespace ultradope {

void DrawHydroWorld(const GameSession& session, HydroSocket targetSocket,
                    bool validationAttempted, const HydroValidationResult& validation,
                    float animationTime, bool moving);

}  // namespace ultradope
