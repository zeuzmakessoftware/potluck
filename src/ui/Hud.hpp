#pragma once

#include "domain/GameSession.hpp"
#include "world/FarmLayout.hpp"
#include "world/HydroLayout.hpp"

#include <string>

namespace ultradope {

void DrawOutdoorHud(const GameSession& session, FarmInteractionKind interaction,
                    int targetPlot, const std::string& toast, float toastTime);
void DrawHydroHud(const GameSession& session, HydroInteractionKind interaction,
                  HydroSocket targetSocket, const HydroValidationResult& validation,
                  bool validationAttempted, const std::string& toast, float toastTime);

}  // namespace ultradope
