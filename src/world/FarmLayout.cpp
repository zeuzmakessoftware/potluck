#include "world/FarmLayout.hpp"

#include <algorithm>
#include <cmath>

namespace ultradope {
namespace {
struct InteractionPoint {
    FarmInteractionKind kind;
    Vector2 position;
    const char* prompt;
};

constexpr InteractionPoint kInteractions[] = {
    {FarmInteractionKind::House, {0.0F, 11.0F}, "E - Sleep until morning"},
    {FarmInteractionKind::Shop, {-14.5F, 8.0F}, "E - Milo's seed stand"},
    {FarmInteractionKind::ShippingBin, {14.0F, 7.0F}, "E - Open shipping bin"},
    {FarmInteractionKind::WaterTank, {13.5F, -3.5F}, "R - Refill watering can"},
    {FarmInteractionKind::Juniper, {-4.0F, 8.5F}, "E - Talk to Juniper"},
    {FarmInteractionKind::Milo, {-12.0F, 6.5F}, "E - Talk to Milo"},
    {FarmInteractionKind::Tessa, {11.0F, -9.0F}, "E - Talk to Tessa"},
    {FarmInteractionKind::HydroTransport, {18.0F, -5.0F}, "E - Enter hydroponics workshop"}
};

float DistanceSquared(Vec2 a, Vector2 b) {
    const float dx = a.x - b.x;
    const float dz = a.y - b.y;
    return dx * dx + dz * dz;
}

bool Inside(float x, float z, float minX, float maxX, float minZ, float maxZ) {
    return x > minX && x < maxX && z > minZ && z < maxZ;
}
}  // namespace

Vector3 PlotPosition(std::size_t index) {
    const float column = static_cast<float>(index % 6U);
    const float row = static_cast<float>(index / 6U);
    return {-7.5F + column * 3.0F, 0.18F, -4.5F + row * 3.0F};
}

Vector3 PlayerPosition3D(const PlayerState& player) {
    return {player.position.x, 0.0F, player.position.y};
}

Vec2 FarmEntrySpawn() { return {0.0F, 9.0F}; }

Vec2 FarmHydroReturnSpawn() { return {16.0F, -5.0F}; }

int FindTargetPlot(const GameSession& session) {
    const float frontX = session.player.position.x + std::sin(session.player.facingRadians) * 2.1F;
    const float frontZ = session.player.position.y + std::cos(session.player.facingRadians) * 2.1F;
    int best = -1;
    float bestDistance = 2.5F;
    for (std::size_t index = 0; index < session.farm.plots.size(); ++index) {
        const Vector3 position = PlotPosition(index);
        const float dx = frontX - position.x;
        const float dz = frontZ - position.z;
        const float distance = dx * dx + dz * dz;
        if (distance < bestDistance) {
            bestDistance = distance;
            best = static_cast<int>(index);
        }
    }
    return best;
}

FarmInteractionKind FindNearbyFarmInteraction(const GameSession& session) {
    FarmInteractionKind best = FarmInteractionKind::None;
    float bestDistance = 5.0F;
    for (const auto& interaction : kInteractions) {
        if (interaction.kind == FarmInteractionKind::HydroTransport &&
            !session.progression.hydroUnlocked) {
            continue;
        }
        const float distance = DistanceSquared(session.player.position, interaction.position);
        if (distance < bestDistance) {
            bestDistance = distance;
            best = interaction.kind;
        }
    }
    return best;
}

const char* FarmInteractionPrompt(FarmInteractionKind kind) {
    for (const auto& interaction : kInteractions) {
        if (interaction.kind == kind) return interaction.prompt;
    }
    return "";
}

void MovePlayerOnFarm(PlayerState& player, Vec2 delta) {
    const Vec2 previous = player.position;
    player.position.x = std::clamp(player.position.x + delta.x, -21.5F, 21.5F);
    player.position.y = std::clamp(player.position.y + delta.y, -20.5F, 20.5F);

    if (Inside(player.position.x, player.position.y, -5.5F, 5.5F, 11.0F, 19.0F) ||
        Inside(player.position.x, player.position.y, -19.5F, -11.5F, 8.0F, 14.5F)) {
        player.position = previous;
    }
}

}  // namespace ultradope
