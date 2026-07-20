#include "render/WorldRenderer.hpp"

#include "render/CharacterRenderer.hpp"
#include "render/CropRenderer.hpp"
#include "render/Palette.hpp"
#include "render/PrimitiveDraw.hpp"
#include "world/FarmLayout.hpp"

#include <algorithm>
#include <cmath>

namespace ultradope {
namespace {
void DrawPlots(const GameSession& session, int targetPlot) {
    for (std::size_t index = 0; index < session.farm.plots.size(); ++index) {
        const auto& plot = session.farm.plots[index];
        const Vector3 position = PlotPosition(index);
        const Color soil = plot.wateredToday ? palette::WetSoil : palette::Soil;
        // Every plot is a rough timber grow bed; state changes the contents,
        // not its silhouette.  The dense repeating grid anchors the farm view.
        DrawCube({position.x, 0.20F, position.z - 1.13F}, 2.65F, 0.34F, 0.18F,
                 Color{68, 43, 27, 255});
        DrawCube({position.x, 0.20F, position.z + 1.13F}, 2.65F, 0.34F, 0.18F,
                 Color{82, 50, 29, 255});
        DrawCube({position.x - 1.24F, 0.20F, position.z}, 0.18F, 0.34F, 2.42F,
                 Color{72, 43, 25, 255});
        DrawCube({position.x + 1.24F, 0.20F, position.z}, 0.18F, 0.34F, 2.42F,
                 Color{93, 56, 31, 255});
        if (plot.tilled) {
            DrawCube(position, 2.3F, 0.28F, 2.15F, soil);
            for (int furrow = -1; furrow <= 1; ++furrow) {
                DrawCube({position.x + static_cast<float>(furrow) * 0.55F, 0.35F, position.z},
                         0.1F, 0.08F, 2.0F, Color{82, 55, 40, 255});
            }
        } else {
            DrawCube(position, 2.3F, 0.12F, 2.15F, palette::GrassDark);
        }
        if (static_cast<int>(index) == targetPlot) {
            Color highlight = IsMature(plot) ? palette::Gold : Color{239, 245, 208, 255};
            DrawCubeWires({position.x, 0.43F, position.z}, 2.5F, 0.36F, 2.5F, highlight);
        }
        DrawCrop(plot, position, index);
    }
}

void DrawDecorations() {
    DrawTree({-18.0F, 0.0F, -13.0F}, 1.1F, Color{60, 124, 67, 255});
    DrawTree({-14.0F, 0.0F, -16.0F}, 0.8F, Color{76, 145, 74, 255});
    DrawTree({18.0F, 0.0F, -14.0F}, 1.0F, Color{66, 132, 65, 255});
    DrawTree({18.0F, 0.0F, 15.0F}, 0.9F, Color{85, 143, 73, 255});
    DrawTree({10.0F, 0.0F, 17.0F}, 0.7F, Color{76, 133, 70, 255});
    for (int i = 0; i < 34; ++i) {
        const float x = -19.0F + static_cast<float>((i * 7) % 36);
        const float z = -18.0F + static_cast<float>((i * 11) % 34);
        DrawCylinder({x, 0.0F, z}, 0.025F, 0.06F, 0.28F + static_cast<float>(i % 3) * 0.12F,
                     4, i % 4 == 0 ? Color{119, 94, 40, 255} : palette::GrassDark);
    }
    // Work-yard clutter: wheelbarrow, sacks and split logs.
    DrawCube({-8.9F, 0.46F, 12.0F}, 1.55F, 0.22F, 0.85F, Color{112, 45, 30, 255});
    DrawCylinder({-9.55F, 0.0F, 12.0F}, 0.32F, 0.32F, 0.12F, 8, palette::Ink);
    DrawCube({-8.0F, 0.34F, 12.0F}, 1.7F, 0.09F, 0.09F, palette::Wood);
    for (int i = 0; i < 4; ++i) {
        DrawCylinder({7.3F + static_cast<float>(i) * 0.38F, 0.12F, 13.1F},
                     0.14F, 0.14F, 1.5F, 7, Color{92, 60, 34, 255});
    }
}

void DrawGroundDetail() {
    for (int i = 0; i < 42; ++i) {
        const float x = -20.0F + static_cast<float>((i * 97) % 400) / 10.0F;
        const float z = -19.0F + static_cast<float>((i * 53) % 380) / 10.0F;
        const float size = 0.10F + static_cast<float>(i % 4) * 0.045F;
        DrawCube({x, 0.025F, z}, size, 0.035F, size * 0.65F,
                 i % 3 == 0 ? Color{121, 90, 49, 255} : Color{44, 57, 30, 255});
    }
}

void DrawRain(float animationTime) {
    for (int i = 0; i < 90; ++i) {
        const float x = -22.0F + static_cast<float>((i * 37) % 440) / 10.0F;
        const float z = -21.0F + static_cast<float>((i * 61) % 420) / 10.0F;
        const float y = std::fmod(animationTime * 8.0F + static_cast<float>(i) * 0.71F, 8.0F);
        DrawLine3D({x, y, z}, {x - 0.08F, y - 0.65F, z}, Color{150, 205, 230, 175});
    }
}
}  // namespace

Color SkyColorForTime(const GameSession& session) {
    const float hour = session.calendar.minuteOfDay / 60.0F;
    if (hour < 7.0F) return Color{183, 92, 46, 255};
    if (hour > 19.0F) return Color{55, 57, 64, 255};
    if (session.calendar.weather == Weather::Rainy) return Color{94, 104, 95, 255};
    return Color{185, 132, 67, 255};
}

void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime,
                      const PlayerRenderState& player, float doorOpenFraction) {
    DrawPlane({0.0F, -0.02F, 0.0F}, {46.0F, 44.0F}, palette::Grass);
    DrawCube({0.0F, 0.03F, 7.5F}, 3.0F, 0.08F, 8.0F, palette::Path);
    DrawCube({-7.0F, 0.03F, 8.0F}, 15.0F, 0.08F, 2.0F, palette::Path);
    DrawCube({8.0F, 0.03F, 7.0F}, 14.0F, 0.08F, 1.8F, palette::Path);
    DrawGroundDetail();
    DrawFarmhouse(doorOpenFraction);
    DrawSeedKiosk();
    DrawShippingBin();
    DrawWaterTank();
    DrawFence();
    DrawDecorations();
    if (session.progression.hydroUnlocked) {
        DrawCube({18.0F, 1.4F, -5.0F}, 3.8F, 2.8F, 5.0F, Color{70, 91, 84, 255});
        DrawCube({18.0F, 2.95F, -5.0F}, 4.2F, 0.3F, 5.4F, palette::Gold);
        DrawCube({16.05F, 1.1F, -5.0F}, 0.18F, 2.2F, 2.0F, Color{55, 66, 62, 255});
        DrawCubeWires({18.0F, 1.4F, -5.0F}, 3.8F, 2.8F, 5.0F, palette::Cream);
    }
    DrawPlots(session, targetPlot);
    DrawNpcCharacter({-4.0F, 0.0F, 8.5F}, Color{57, 109, 75, 255}, palette::Cream,
                     "Juniper", animationTime);
    DrawNpcCharacter({-12.0F, 0.0F, 6.5F}, Color{174, 91, 53, 255}, palette::Gold,
                     "Milo", animationTime);
    DrawNpcCharacter({11.0F, 0.0F, -9.0F}, Color{112, 76, 151, 255}, palette::Violet,
                     "Tessa", animationTime);
    DrawPlayerCharacter(player);
    if (session.calendar.weather == Weather::Rainy) DrawRain(animationTime);
}

void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime, bool moving) {
    DrawOutdoorWorld(session, targetPlot, animationTime,
                     PlayerRenderState{session.player.position, session.player.facingRadians,
                                       animationTime, moving ? 1.0F : 0.0F, 0.0F},
                     0.0F);
}

}  // namespace ultradope
