#include "render/PrimitiveDraw.hpp"

#include "render/Palette.hpp"

#include <algorithm>
#include <rlgl.h>

namespace ultradope {
namespace {
void DrawBoard(Vector3 position, Vector3 size, Color color) {
    DrawCubeV(position, size, color);
    DrawCubeWiresV(position, size, ColorAlpha(palette::Ink, 0.42F));
}

void DrawWindow(float x, float y, float z) {
    DrawCube({x, y, z}, 1.85F, 1.45F, 0.16F, Color{232, 168, 71, 255});
    DrawCube({x, y, z - 0.10F}, 0.10F, 1.55F, 0.12F, palette::Wood);
    DrawCube({x, y, z - 0.10F}, 1.95F, 0.10F, 0.12F, palette::Wood);
    DrawCubeWires({x, y, z}, 1.92F, 1.52F, 0.18F, palette::Ink);
}

void DrawCrate(Vector3 p, float scale) {
    DrawBoard({p.x, p.y + 0.45F * scale, p.z},
              {1.15F * scale, 0.9F * scale, 1.05F * scale}, Color{104, 65, 35, 255});
    DrawCube({p.x, p.y + 0.45F * scale, p.z - 0.54F * scale},
             0.12F * scale, 0.76F * scale, 0.08F * scale, Color{53, 35, 23, 255});
}
}  // namespace

void DrawFarmhouse(float doorOpenFraction) {
    DrawCube({0.0F, 2.6F, 15.0F}, 10.0F, 5.2F, 7.0F, Color{158, 137, 94, 255});
    DrawCubeWires({0.0F, 2.6F, 15.0F}, 10.0F, 5.2F, 7.0F, Color{65, 43, 28, 255});
    // Uneven roof bands fake rows of sun-baked tile without external textures.
    for (int row = 0; row < 7; ++row) {
        const float z = 11.25F + static_cast<float>(row) * 1.2F;
        const Color tile = row % 2 == 0 ? palette::Roof : Color{96, 40, 28, 255};
        DrawBoard({0.0F, 5.5F + static_cast<float>(row % 2) * 0.05F, z},
                  {11.2F, 0.42F, 1.34F}, tile);
    }
    DrawCube({0.0F, 1.42F, 11.43F}, 2.18F, 2.92F, 0.20F, Color{23, 20, 16, 255});
    DrawBoard({-1.13F, 1.48F, 11.30F}, {0.18F, 3.08F, 0.30F}, palette::Wood);
    DrawBoard({1.13F, 1.48F, 11.30F}, {0.18F, 3.08F, 0.30F}, palette::Wood);
    DrawBoard({0.0F, 2.96F, 11.30F}, {2.44F, 0.18F, 0.30F}, palette::Wood);

    const float doorAngle = 94.0F * std::clamp(doorOpenFraction, 0.0F, 1.0F);
    rlPushMatrix();
    rlTranslatef(1.02F, 1.42F, 11.22F);
    rlRotatef(doorAngle, 0.0F, 1.0F, 0.0F);
    DrawBoard({-1.02F, 0.0F, 0.0F}, {2.04F, 2.84F, 0.28F}, Color{62, 39, 25, 255});
    DrawCube({-1.02F, 0.03F, -0.16F}, 0.13F, 2.30F, 0.08F, Color{35, 26, 18, 255});
    DrawSphere({-1.72F, 0.05F, -0.19F}, 0.09F, palette::Gold);
    rlPopMatrix();
    DrawWindow(-3.0F, 2.8F, 11.40F);
    DrawWindow(3.0F, 2.8F, 11.40F);
    DrawCylinder({3.5F, 5.75F, 16.5F}, 0.45F, 0.45F, 2.6F, 8, Color{69, 47, 35, 255});
    // Stucco scars, weeds and an old porch make the silhouette feel inhabited.
    DrawCube({-4.25F, 0.65F, 11.30F}, 1.0F, 1.0F, 0.08F, Color{104, 96, 67, 255});
    DrawCube({2.05F, 4.5F, 11.30F}, 1.35F, 0.45F, 0.08F, Color{119, 102, 70, 255});
    DrawBoard({0.0F, 0.10F, 10.65F}, {4.1F, 0.20F, 1.35F}, Color{100, 69, 42, 255});
    for (int i = -2; i <= 2; ++i) {
        DrawCylinder({-4.55F + static_cast<float>(i) * 0.28F, 0.0F, 11.2F},
                     0.03F, 0.06F, 0.75F + static_cast<float>((i + 2) % 2) * 0.25F, 5,
                     palette::GrassDark);
    }
}

void DrawSeedKiosk() {
    DrawBoard({-15.5F, 1.4F, 10.5F}, {7.0F, 2.8F, 5.0F}, Color{89, 73, 47, 255});
    for (int strip = 0; strip < 6; ++strip) {
        DrawBoard({-15.5F, 3.22F, 7.6F + static_cast<float>(strip) * 1.08F},
                  {7.8F, 0.30F, 1.18F}, strip % 2 ? Color{137, 54, 31, 255}
                                                   : Color{163, 65, 34, 255});
    }
    DrawBoard({-15.5F, 0.95F, 7.85F}, {6.0F, 1.45F, 0.85F}, Color{65, 43, 28, 255});
    DrawCrate({-17.2F, 0.0F, 6.85F}, 1.0F);
    DrawCrate({-14.2F, 0.0F, 6.85F}, 0.85F);
    DrawCylinder({-12.3F, 0.0F, 7.1F}, 0.55F, 0.48F, 1.35F, 10, Color{71, 80, 59, 255});
    DrawCylinderWires({-12.3F, 0.0F, 7.1F}, 0.55F, 0.48F, 1.35F, 10, palette::Ink);
}

void DrawShippingBin() {
    DrawBoard({14.0F, 1.0F, 7.0F}, {3.3F, 2.0F, 2.7F}, Color{44, 69, 59, 255});
    for (int slat = -1; slat <= 1; ++slat) {
        DrawCube({14.0F + static_cast<float>(slat) * 1.0F, 1.0F, 5.61F},
                 0.08F, 1.75F, 0.06F, Color{25, 39, 33, 255});
    }
    DrawBoard({14.0F, 2.15F, 7.0F}, {3.7F, 0.28F, 3.05F}, Color{156, 100, 32, 255});
}

void DrawWaterTank() {
    DrawCylinder({13.5F, 0.0F, -3.5F}, 1.8F, 1.8F, 4.5F, 12, Color{62, 83, 72, 255});
    DrawCylinderWires({13.5F, 0.0F, -3.5F}, 1.8F, 1.8F, 4.5F, 12, palette::Ink);
    DrawCylinder({13.5F, 4.5F, -3.5F}, 1.8F, 1.8F, 0.2F, 12, Color{105, 93, 52, 255});
    DrawCube({11.7F, 0.6F, -3.5F}, 1.2F, 0.25F, 0.25F, palette::Ink);
    for (int band = 1; band <= 3; ++band) {
        DrawCylinderWires({13.5F, static_cast<float>(band), -3.5F}, 1.82F, 1.82F, 0.05F,
                          12, Color{123, 91, 47, 255});
    }
}

void DrawTree(Vector3 position, float scale, Color leafColor) {
    DrawCylinder(position, 0.35F * scale, 0.5F * scale, 2.8F * scale, 8, palette::Wood);
    const Vector3 crown{position.x, position.y + 3.1F * scale, position.z};
    DrawSphere(crown, 1.45F * scale, leafColor);
    DrawSphere({crown.x - 0.9F * scale, crown.y - 0.2F * scale, crown.z}, 0.95F * scale,
               ColorTint(leafColor, Color{195, 170, 112, 255}));
    DrawSphere({crown.x + 0.8F * scale, crown.y, crown.z + 0.4F * scale}, 1.0F * scale,
               ColorTint(leafColor, Color{150, 140, 90, 255}));
}

void DrawFence() {
    for (int x = -22; x <= 22; x += 4) {
        DrawCube({static_cast<float>(x), 0.8F, -21.5F}, 0.3F, 1.6F, 0.3F, palette::Wood);
        DrawCube({static_cast<float>(x), 0.8F, 21.5F}, 0.3F, 1.6F, 0.3F, palette::Wood);
    }
    DrawCube({0.0F, 0.75F, -21.5F}, 44.0F, 0.2F, 0.2F, palette::Wood);
    DrawCube({0.0F, 1.25F, -21.5F}, 44.0F, 0.2F, 0.2F, palette::Wood);
    DrawCube({0.0F, 0.75F, 21.5F}, 44.0F, 0.2F, 0.2F, palette::Wood);
    DrawCube({0.0F, 1.25F, 21.5F}, 44.0F, 0.2F, 0.2F, palette::Wood);
    for (int z = -18; z <= 18; z += 4) {
        DrawCube({-22.0F, 0.8F, static_cast<float>(z)}, 0.3F, 1.6F, 0.3F, palette::Wood);
        DrawCube({22.0F, 0.8F, static_cast<float>(z)}, 0.3F, 1.6F, 0.3F, palette::Wood);
    }
    DrawCube({-22.0F, 0.75F, 0.0F}, 0.2F, 0.2F, 43.0F, palette::Wood);
    DrawCube({22.0F, 0.75F, 0.0F}, 0.2F, 0.2F, 43.0F, palette::Wood);
}

}  // namespace ultradope
