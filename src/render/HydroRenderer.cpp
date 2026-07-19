#include "render/HydroRenderer.hpp"

#include "render/CharacterRenderer.hpp"
#include "render/Palette.hpp"
#include "world/HydroLayout.hpp"

#include <raylib.h>

#include <cmath>

namespace ultradope {
namespace {
std::size_t Index(HydroSocket socket) { return static_cast<std::size_t>(socket); }

bool IsChannel(HydroSocket socket) {
    return socket == HydroSocket::LeftChannel || socket == HydroSocket::CenterChannel ||
           socket == HydroSocket::RightChannel;
}

bool IsLight(HydroSocket socket) {
    return socket == HydroSocket::LeftLight || socket == HydroSocket::CenterLight ||
           socket == HydroSocket::RightLight;
}

bool SocketRelevantToFailure(HydroSocket socket, const HydroValidationResult& validation) {
    if (socket == HydroSocket::ReservoirBase && !validation.hasExactlyOneReservoir) return true;
    if ((socket == HydroSocket::LeftPump || socket == HydroSocket::RightPump) &&
        (!validation.hasExactlyOnePump || !validation.allChannelsReachable)) return true;
    if (IsChannel(socket) && (!validation.hasMinimumChannels || !validation.allChannelsReachable)) {
        return true;
    }
    return IsLight(socket) && !validation.allChannelsLit;
}

void DrawSocket(HydroSocket socket, bool targeted, bool failed) {
    const Vector3 position = HydroSocketPosition(socket);
    const Color color = targeted ? palette::Gold : failed ? Color{210, 72, 63, 255}
                                                       : Color{150, 174, 166, 170};
    if (IsLight(socket)) {
        DrawCubeWires(position, 2.6F, 0.3F, 1.5F, color);
    } else if (IsChannel(socket)) {
        DrawCubeWires(position, 4.8F, 0.35F, 1.7F, color);
    } else {
        DrawCubeWires(position, 2.0F, 0.35F, 2.0F, color);
    }
}

void DrawReservoir(Vector3 position) {
    DrawCylinder({position.x, 0.25F, position.z}, 1.25F, 1.25F, 2.5F, 18,
                 Color{87, 139, 151, 255});
    DrawCylinder({position.x, 2.75F, position.z}, 1.25F, 1.25F, 0.15F, 18,
                 Color{129, 198, 214, 255});
    DrawCylinderWires({position.x, 0.25F, position.z}, 1.25F, 1.25F, 2.5F, 18,
                      palette::Cream);
}

void DrawPump(Vector3 position) {
    DrawCube({position.x, 0.65F, position.z}, 1.7F, 1.3F, 1.45F, Color{66, 82, 78, 255});
    DrawCylinder({position.x, 1.3F, position.z}, 0.52F, 0.52F, 0.8F, 12,
                 Color{208, 117, 56, 255});
    DrawCubeWires({position.x, 0.65F, position.z}, 1.7F, 1.3F, 1.45F, palette::Cream);
}

void DrawChannel(Vector3 position) {
    DrawCube(position, 4.6F, 0.45F, 1.55F, Color{224, 229, 217, 255});
    DrawCube({position.x, position.y + 0.27F, position.z}, 4.2F, 0.12F, 1.1F,
             Color{65, 133, 151, 255});
    for (int hole = -2; hole <= 2; ++hole) {
        DrawCylinder({position.x + static_cast<float>(hole) * 0.8F, position.y + 0.35F,
                      position.z}, 0.14F, 0.14F, 0.12F, 8, palette::GrassDark);
    }
}

void DrawLight(Vector3 position, bool powered, float animationTime) {
    DrawCube({position.x, 6.2F, position.z}, 0.08F, 3.8F, 0.08F, Color{73, 79, 75, 255});
    const Color housing = powered ? Color{250, 201, 92, 255} : Color{106, 113, 108, 255};
    DrawCube(position, 2.5F, 0.28F, 1.35F, housing);
    DrawCubeWires(position, 2.5F, 0.28F, 1.35F, palette::Cream);
    if (powered) {
        const float pulse = 0.12F + std::sin(animationTime * 2.0F) * 0.025F;
        DrawCube({position.x, 3.2F, position.z}, 2.35F, 1.9F, 1.2F,
                 ColorAlpha(Color{255, 220, 120, 255}, pulse));
    }
}

void DrawInstalledEquipment(const GameSession& session, float animationTime) {
    const bool powered = session.hydro.completed || ValidateHydroBuild(session.hydro).IsValid();
    for (std::size_t index = 0; index < kHydroSocketCount; ++index) {
        const HydroSocket socket = static_cast<HydroSocket>(index);
        const Vector3 position = HydroSocketPosition(socket);
        switch (session.hydro.equipment[index]) {
            case HydroEquipment::Reservoir: DrawReservoir(position); break;
            case HydroEquipment::Pump: DrawPump(position); break;
            case HydroEquipment::GrowChannel: DrawChannel(position); break;
            case HydroEquipment::GrowLight: DrawLight(position, powered, animationTime); break;
            case HydroEquipment::None: break;
        }
    }
}

void DrawPipeRuns() {
    const Color pipe = Color{79, 139, 145, 255};
    DrawCylinder({-4.7F, 0.45F, 0.0F}, 0.13F, 0.13F, 2.6F, 8, pipe);
    DrawCube({-2.0F, 0.42F, 0.0F}, 4.0F, 0.22F, 0.22F, pipe);
    DrawCube({0.0F, 0.42F, 0.0F}, 0.22F, 0.22F, 8.0F, pipe);
}

void DrawRoom() {
    DrawPlane({0.0F, 0.0F, 0.0F}, {20.0F, 16.0F}, Color{56, 55, 46, 255});
    DrawCube({0.0F, 4.0F, -8.0F}, 20.0F, 8.0F, 0.25F, Color{128, 125, 101, 255});
    DrawCube({-10.0F, 4.0F, 0.0F}, 0.25F, 8.0F, 16.0F, Color{108, 108, 91, 255});
    DrawCube({10.0F, 4.0F, 0.0F}, 0.25F, 8.0F, 16.0F, Color{108, 108, 91, 255});
    for (int stripe = 0; stripe < 5; ++stripe) {
        DrawCube({-7.5F + static_cast<float>(stripe) * 3.7F, 0.035F, 0.0F},
                 0.10F, 0.05F, 16.0F, Color{36, 38, 32, 255});
    }
    for (int x = -8; x <= 8; x += 4) {
        DrawCube({static_cast<float>(x), 7.6F, 0.0F}, 0.22F, 0.22F, 16.0F,
                 Color{68, 75, 70, 255});
    }
    DrawCube({0.0F, 3.0F, 7.9F}, 3.0F, 6.0F, 0.25F, Color{57, 69, 65, 255});
    DrawCube({7.0F, 1.15F, -5.2F}, 2.4F, 2.3F, 1.5F, Color{52, 64, 61, 255});
    DrawCube({7.0F, 2.25F, -5.2F}, 1.7F, 0.18F, 1.0F, Color{94, 205, 153, 255});
    DrawCubeWires({7.0F, 1.15F, -5.2F}, 2.4F, 2.3F, 1.5F, palette::Cream);
}
}  // namespace

void DrawHydroWorld(const GameSession& session, HydroSocket targetSocket,
                    bool validationAttempted, const HydroValidationResult& validation,
                    float animationTime, bool moving) {
    DrawRoom();
    DrawPipeRuns();
    for (std::size_t index = 0; index < kHydroSocketCount; ++index) {
        if (session.hydro.equipment[index] != HydroEquipment::None) continue;
        const HydroSocket socket = static_cast<HydroSocket>(index);
        DrawSocket(socket, socket == targetSocket,
                   validationAttempted && SocketRelevantToFailure(socket, validation));
    }
    DrawInstalledEquipment(session, animationTime);
    if (targetSocket != HydroSocket::Count &&
        session.hydro.equipment[Index(targetSocket)] != HydroEquipment::None) {
        DrawSocket(targetSocket, true, false);
    }
    DrawPlayerCharacter(session.player, animationTime, moving);
}

}  // namespace ultradope
