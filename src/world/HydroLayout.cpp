#include "world/HydroLayout.hpp"

#include <algorithm>
#include <cmath>

namespace ultradope {
namespace {
float DistanceSquared(Vec2 a, Vector2 b) {
    const float dx = a.x - b.x;
    const float dz = a.y - b.y;
    return dx * dx + dz * dz;
}

bool Inside(float x, float z, float minX, float maxX, float minZ, float maxZ) {
    return x > minX && x < maxX && z > minZ && z < maxZ;
}
}  // namespace

Vector3 HydroSocketPosition(HydroSocket socket) {
    switch (socket) {
        case HydroSocket::ReservoirBase: return {-6.0F, 0.25F, 0.0F};
        case HydroSocket::LeftPump: return {-3.4F, 0.25F, -2.4F};
        case HydroSocket::RightPump: return {-3.4F, 0.25F, 2.4F};
        case HydroSocket::LeftChannel: return {1.2F, 0.75F, -4.0F};
        case HydroSocket::CenterChannel: return {1.2F, 0.75F, 0.0F};
        case HydroSocket::RightChannel: return {1.2F, 0.75F, 4.0F};
        case HydroSocket::LeftLight: return {1.2F, 4.3F, -4.0F};
        case HydroSocket::CenterLight: return {1.2F, 4.3F, 0.0F};
        case HydroSocket::RightLight: return {1.2F, 4.3F, 4.0F};
        case HydroSocket::Count: return {};
    }
    return {};
}

Vec2 HydroEntrySpawn() { return {0.0F, 6.0F}; }

HydroSocket FindTargetHydroSocket(const GameSession& session) {
    const Vec2 probe = {
        session.player.position.x + std::sin(session.player.facingRadians) * 2.2F,
        session.player.position.y + std::cos(session.player.facingRadians) * 2.2F
    };
    HydroSocket best = HydroSocket::Count;
    float bestDistance = 3.2F;
    for (std::size_t index = 0; index < kHydroSocketCount; ++index) {
        const HydroSocket socket = static_cast<HydroSocket>(index);
        if (!IsCompatibleHydroPlacement(socket, session.hydro.selectedEquipment)) continue;
        const Vector3 position = HydroSocketPosition(socket);
        const float distance = DistanceSquared(probe, {position.x, position.z});
        if (distance < bestDistance) {
            bestDistance = distance;
            best = socket;
        }
    }
    return best;
}

HydroInteractionKind FindNearbyHydroInteraction(const PlayerState& player) {
    if (DistanceSquared(player.position, {7.0F, -5.2F}) < 5.0F) {
        return HydroInteractionKind::Console;
    }
    if (DistanceSquared(player.position, {0.0F, 7.2F}) < 5.0F) {
        return HydroInteractionKind::ExitDoor;
    }
    return HydroInteractionKind::None;
}

const char* HydroInteractionPrompt(HydroInteractionKind kind) {
    switch (kind) {
        case HydroInteractionKind::Console: return "E - Validate hydroponics build";
        case HydroInteractionKind::ExitDoor: return "E - Return to outdoor farm";
        case HydroInteractionKind::None: return "";
    }
    return "";
}

void MovePlayerInHydro(PlayerState& player, Vec2 delta) {
    const Vec2 previous = player.position;
    player.position.x = std::clamp(player.position.x + delta.x, -9.0F, 9.0F);
    player.position.y = std::clamp(player.position.y + delta.y, -7.0F, 7.0F);

    if (Inside(player.position.x, player.position.y, 5.8F, 8.4F, -6.5F, -4.2F) ||
        Inside(player.position.x, player.position.y, -7.4F, -4.8F, -1.6F, 1.6F)) {
        player.position = previous;
    }
}

}  // namespace ultradope
