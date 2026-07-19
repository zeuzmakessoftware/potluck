#include "render/CharacterRenderer.hpp"

#include "render/Palette.hpp"

#include <cmath>

namespace ultradope {
namespace {
void DrawLimb(Vector3 start, Vector3 end, float radius, Color color) {
    const Vector3 delta{end.x - start.x, end.y - start.y, end.z - start.z};
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    DrawCylinderEx(start, end, radius, radius * 0.82F, 6, color);
    if (length > 0.0F) DrawSphere(end, radius * 1.05F, color);
}

void DrawPerson(Vector3 p, Color clothing, Color accent, float bob) {
    p.y += bob;
    const Color denim{35, 42, 43, 255};
    const Color skin{174, 119, 78, 255};
    // Oversized head, blocky hands and chunky shoes give the cast a readable,
    // deliberately low-poly silhouette from the high gameplay camera.
    DrawCylinder({p.x, p.y + 0.72F, p.z}, 0.38F, 0.29F, 0.92F, 6, clothing);
    DrawCube({p.x, p.y + 0.55F, p.z - 0.02F}, 0.72F, 0.72F, 0.48F, clothing);
    DrawSphere({p.x, p.y + 1.64F, p.z}, 0.39F, skin);
    DrawSphere({p.x, p.y + 1.91F, p.z - 0.02F}, 0.35F, accent);
    DrawCube({p.x, p.y + 1.87F, p.z - 0.28F}, 0.78F, 0.10F, 0.34F, accent);
    DrawLimb({p.x - 0.33F, p.y + 1.10F, p.z},
             {p.x - 0.49F, p.y + 0.60F, p.z - 0.03F}, 0.105F, clothing);
    DrawLimb({p.x + 0.33F, p.y + 1.10F, p.z},
             {p.x + 0.49F, p.y + 0.60F, p.z - 0.03F}, 0.105F, clothing);
    DrawSphere({p.x - 0.49F, p.y + 0.52F, p.z - 0.03F}, 0.13F, skin);
    DrawSphere({p.x + 0.49F, p.y + 0.52F, p.z - 0.03F}, 0.13F, skin);
    DrawLimb({p.x - 0.18F, p.y + 0.35F, p.z}, {p.x - 0.2F, p.y - 0.05F, p.z},
             0.13F, denim);
    DrawLimb({p.x + 0.18F, p.y + 0.35F, p.z}, {p.x + 0.2F, p.y - 0.05F, p.z},
             0.13F, denim);
    DrawCube({p.x - 0.2F, p.y - 0.08F, p.z - 0.10F}, 0.34F, 0.16F, 0.55F, palette::Ink);
    DrawCube({p.x + 0.2F, p.y - 0.08F, p.z - 0.10F}, 0.34F, 0.16F, 0.55F, palette::Ink);
    DrawCircle3D({p.x, 0.015F, p.z}, 0.48F, {1.0F, 0.0F, 0.0F}, 90.0F, Color{30, 35, 28, 75});
}
}  // namespace

void DrawPlayerCharacter(const PlayerState& player, float animationTime, bool moving) {
    const float bob = moving ? std::sin(animationTime * 9.0F) * 0.055F : 0.0F;
    DrawPerson({player.position.x, 0.58F, player.position.y},
               Color{62, 107, 145, 255}, palette::Gold, bob);
    const Vector3 direction{std::sin(player.facingRadians), 0.0F, std::cos(player.facingRadians)};
    DrawSphere({player.position.x + direction.x * 0.36F, 2.2F + bob,
                player.position.y + direction.z * 0.36F}, 0.08F, palette::Ink);
}

void DrawNpcCharacter(Vector3 position, Color clothing, Color accent, const char*) {
    DrawPerson({position.x, 0.58F, position.z}, clothing, accent, 0.0F);
}

}  // namespace ultradope
