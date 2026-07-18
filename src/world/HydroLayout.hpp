#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

namespace ultradope {

enum class HydroInteractionKind { None, Console, ExitDoor };

Vector3 HydroSocketPosition(HydroSocket socket);
Vec2 HydroEntrySpawn();
HydroSocket FindTargetHydroSocket(const GameSession& session);
HydroInteractionKind FindNearbyHydroInteraction(const PlayerState& player);
const char* HydroInteractionPrompt(HydroInteractionKind kind);
void MovePlayerInHydro(PlayerState& player, Vec2 delta);

}  // namespace ultradope
