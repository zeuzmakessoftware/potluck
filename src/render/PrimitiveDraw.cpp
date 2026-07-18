#include "render/PrimitiveDraw.hpp"

#include "render/Palette.hpp"

namespace ultradope {

void DrawFarmhouse() {
    DrawCube({0.0F, 2.6F, 15.0F}, 10.0F, 5.2F, 7.0F, Color{225, 197, 145, 255});
    DrawCubeWires({0.0F, 2.6F, 15.0F}, 10.0F, 5.2F, 7.0F, palette::Wood);
    DrawCube({0.0F, 5.5F, 15.0F}, 11.0F, 1.2F, 8.0F, palette::Roof);
    DrawCube({0.0F, 1.4F, 11.42F}, 2.0F, 2.8F, 0.25F, palette::Wood);
    DrawCube({-3.0F, 2.8F, 11.4F}, 1.8F, 1.5F, 0.2F, Color{255, 212, 104, 255});
    DrawCube({3.0F, 2.8F, 11.4F}, 1.8F, 1.5F, 0.2F, Color{255, 212, 104, 255});
    DrawCylinder({3.5F, 6.4F, 16.5F}, 0.45F, 0.45F, 2.6F, 8, palette::Wood);
}

void DrawSeedKiosk() {
    DrawCube({-15.5F, 1.4F, 10.5F}, 7.0F, 2.8F, 5.0F, Color{210, 151, 78, 255});
    DrawCube({-15.5F, 3.2F, 10.0F}, 7.8F, 0.35F, 6.0F, Color{229, 112, 64, 255});
    DrawCube({-15.5F, 1.0F, 7.85F}, 6.0F, 1.4F, 0.8F, palette::Wood);
    DrawCube({-17.0F, 0.45F, 6.9F}, 1.2F, 0.9F, 1.2F, palette::Gold);
    DrawCube({-14.0F, 0.45F, 6.9F}, 1.2F, 0.9F, 1.2F, palette::LeafLight);
}

void DrawShippingBin() {
    DrawCube({14.0F, 1.0F, 7.0F}, 3.3F, 2.0F, 2.7F, Color{68, 104, 87, 255});
    DrawCubeWires({14.0F, 1.0F, 7.0F}, 3.3F, 2.0F, 2.7F, palette::Cream);
    DrawCube({14.0F, 2.15F, 7.0F}, 3.6F, 0.3F, 3.0F, palette::Gold);
}

void DrawWaterTank() {
    DrawCylinder({13.5F, 0.0F, -3.5F}, 1.8F, 1.8F, 4.5F, 16, Color{103, 151, 159, 255});
    DrawCylinder({13.5F, 4.5F, -3.5F}, 1.8F, 1.8F, 0.2F, 16, palette::Water);
    DrawCube({11.7F, 0.6F, -3.5F}, 1.2F, 0.25F, 0.25F, palette::Ink);
}

void DrawTree(Vector3 position, float scale, Color leafColor) {
    DrawCylinder(position, 0.35F * scale, 0.5F * scale, 2.8F * scale, 8, palette::Wood);
    const Vector3 crown{position.x, position.y + 3.1F * scale, position.z};
    DrawSphere(crown, 1.7F * scale, leafColor);
    DrawSphere({crown.x - 0.9F * scale, crown.y - 0.2F * scale, crown.z}, 1.1F * scale, leafColor);
    DrawSphere({crown.x + 0.8F * scale, crown.y, crown.z + 0.4F * scale}, 1.15F * scale, leafColor);
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
