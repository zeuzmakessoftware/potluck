#include "render/WorldRenderer.hpp"

#include "render/CharacterRenderer.hpp"
#include "render/CropRenderer.hpp"
#include "render/Palette.hpp"
#include "render/PrimitiveDraw.hpp"
#include "world/FarmLayout.hpp"

#include <rlgl.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ultradope {
namespace {
constexpr int GrassTextureSize = 128;
constexpr float GrassTextureRepeats = 7.0F;
Texture2D grassTexture{};

float HashValue(int x, int y, int seed) {
    std::uint32_t value = static_cast<std::uint32_t>(x) * 374761393U;
    value += static_cast<std::uint32_t>(y) * 668265263U;
    value += static_cast<std::uint32_t>(seed) * 2246822519U;
    value = (value ^ (value >> 13U)) * 1274126177U;
    value ^= value >> 16U;
    return static_cast<float>(value & 0x00FFFFFFU) / 16777215.0F;
}

float SmoothStep(float value) {
    return value * value * (3.0F - 2.0F * value);
}

float PeriodicNoise(int x, int y, int cells, int seed) {
    const float sampleX = static_cast<float>(x * cells) / static_cast<float>(GrassTextureSize);
    const float sampleY = static_cast<float>(y * cells) / static_cast<float>(GrassTextureSize);
    const int x0 = static_cast<int>(std::floor(sampleX));
    const int y0 = static_cast<int>(std::floor(sampleY));
    const int x1 = (x0 + 1) % cells;
    const int y1 = (y0 + 1) % cells;
    const float blendX = SmoothStep(sampleX - static_cast<float>(x0));
    const float blendY = SmoothStep(sampleY - static_cast<float>(y0));
    const float top = HashValue(x0, y0, seed) * (1.0F - blendX) +
                      HashValue(x1, y0, seed) * blendX;
    const float bottom = HashValue(x0, y1, seed) * (1.0F - blendX) +
                         HashValue(x1, y1, seed) * blendX;
    return top * (1.0F - blendY) + bottom * blendY;
}

Color BlendColor(Color from, Color to, float amount) {
    amount = std::clamp(amount, 0.0F, 1.0F);
    return Color{
        static_cast<unsigned char>(static_cast<float>(from.r) +
                                   static_cast<float>(to.r - from.r) * amount),
        static_cast<unsigned char>(static_cast<float>(from.g) +
                                   static_cast<float>(to.g - from.g) * amount),
        static_cast<unsigned char>(static_cast<float>(from.b) +
                                   static_cast<float>(to.b - from.b) * amount),
        255};
}

Color GrassColor(float value) {
    if (value < 0.32F) {
        return BlendColor(palette::GrassDark, palette::Grass, value / 0.32F);
    }
    if (value < 0.72F) {
        return BlendColor(palette::Grass, palette::GrassFaded, (value - 0.32F) / 0.40F);
    }
    return BlendColor(palette::GrassFaded, palette::GrassDry, (value - 0.72F) / 0.28F);
}

void DrawGrassPlane() {
    if (grassTexture.id == 0) {
        DrawPlane({0.0F, -0.02F, 0.0F}, {46.0F, 44.0F}, palette::Grass);
        return;
    }

    rlSetTexture(grassTexture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0.0F, 1.0F, 0.0F);
    rlTexCoord2f(0.0F, 0.0F);
    rlVertex3f(-23.0F, -0.02F, -22.0F);
    rlTexCoord2f(0.0F, GrassTextureRepeats);
    rlVertex3f(-23.0F, -0.02F, 22.0F);
    rlTexCoord2f(GrassTextureRepeats, GrassTextureRepeats);
    rlVertex3f(23.0F, -0.02F, 22.0F);
    rlTexCoord2f(GrassTextureRepeats, 0.0F);
    rlVertex3f(23.0F, -0.02F, -22.0F);
    rlEnd();
    rlSetTexture(0);
}

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

void DrawRain(float animationTime) {
    for (int i = 0; i < 90; ++i) {
        const float x = -22.0F + static_cast<float>((i * 37) % 440) / 10.0F;
        const float z = -21.0F + static_cast<float>((i * 61) % 420) / 10.0F;
        const float y = std::fmod(animationTime * 8.0F + static_cast<float>(i) * 0.71F, 8.0F);
        DrawLine3D({x, y, z}, {x - 0.08F, y - 0.65F, z}, Color{150, 205, 230, 175});
    }
}
}  // namespace

bool LoadWorldRendererAssets() {
    if (grassTexture.id != 0) return true;

    Image image = GenImageColor(GrassTextureSize, GrassTextureSize, palette::Grass);
    if (image.data == nullptr) return false;

    auto* pixels = static_cast<Color*>(image.data);
    for (int y = 0; y < GrassTextureSize; ++y) {
        for (int x = 0; x < GrassTextureSize; ++x) {
            const float broad = PeriodicNoise(x, y, 5, 17);
            const float medium = PeriodicNoise(x, y, 11, 41);
            const float grain = PeriodicNoise(x, y, 27, 83);
            pixels[y * GrassTextureSize + x] =
                GrassColor(broad * 0.56F + medium * 0.30F + grain * 0.14F);
        }
    }

    // Short, softly blended flecks keep the surface grassy up close without
    // turning the terrain into visible geometric tiles.
    for (int i = 0; i < 260; ++i) {
        const int startX = static_cast<int>(HashValue(i, 19, 101) * GrassTextureSize) %
                           GrassTextureSize;
        const int startY = static_cast<int>(HashValue(i, 47, 131) * GrassTextureSize) %
                           GrassTextureSize;
        const int length = 1 + i % 4;
        const Color accent = i % 4 == 0 ? palette::GrassDry : palette::GrassDark;
        for (int step = 0; step < length; ++step) {
            const int x = (startX + step) % GrassTextureSize;
            const int y = (startY + step / 3) % GrassTextureSize;
            Color& pixel = pixels[y * GrassTextureSize + x];
            pixel = BlendColor(pixel, accent, i % 4 == 0 ? 0.24F : 0.18F);
        }
    }

    grassTexture = LoadTextureFromImage(image);
    UnloadImage(image);
    if (grassTexture.id == 0) return false;

    GenTextureMipmaps(&grassTexture);
    SetTextureFilter(grassTexture, TEXTURE_FILTER_TRILINEAR);
    SetTextureWrap(grassTexture, TEXTURE_WRAP_REPEAT);
    return true;
}

void UnloadWorldRendererAssets() {
    if (grassTexture.id == 0) return;
    UnloadTexture(grassTexture);
    grassTexture = {};
}

Color SkyColorForTime(const GameSession& session) {
    const float hour = session.calendar.minuteOfDay / 60.0F;
    if (hour < 7.0F) return Color{183, 92, 46, 255};
    if (hour > 19.0F) return Color{55, 57, 64, 255};
    if (session.calendar.weather == Weather::Rainy) return Color{94, 104, 95, 255};
    return Color{185, 132, 67, 255};
}

void DrawOutdoorWorld(const GameSession& session, int targetPlot, float animationTime,
                      const PlayerRenderState& player, float doorOpenFraction) {
    DrawGrassPlane();
    DrawCube({0.0F, 0.03F, 7.5F}, 3.0F, 0.08F, 8.0F, palette::Path);
    DrawCube({-7.0F, 0.03F, 8.0F}, 15.0F, 0.08F, 2.0F, palette::Path);
    DrawCube({8.0F, 0.03F, 7.0F}, 14.0F, 0.08F, 1.8F, palette::Path);
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
