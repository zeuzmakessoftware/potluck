#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

#include <cstddef>

namespace ultradope {

enum class FarmInteractionKind {
    None,
    House,
    Shop,
    ShippingBin,
    WaterTank,
    Juniper,
    Milo,
    Tessa,
    HydroTransport
};

Vector3 PlotPosition(std::size_t index);
Vector3 PlayerPosition3D(const PlayerState& player);
Vec2 FarmEntrySpawn();
Vec2 FarmHydroReturnSpawn();
int FindTargetPlot(const GameSession& session);
FarmInteractionKind FindNearbyFarmInteraction(const GameSession& session);
const char* FarmInteractionPrompt(FarmInteractionKind kind);
void MovePlayerOnFarm(PlayerState& player, Vec2 delta);

}  // namespace ultradope
