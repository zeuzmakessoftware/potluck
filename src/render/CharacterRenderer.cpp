#include "render/CharacterRenderer.hpp"

#include "render/Palette.hpp"

#include <cmath>

namespace ultradope {
namespace {
void DrawPerson(Vector3 p, Color clothing, Color accent, float bob) {
    p.y += bob;
    DrawCylinder({p.x, p.y + 0.35F, p.z}, 0.34F, 0.25F, 1.15F, 8, clothing);
    DrawSphere({p.x, p.y + 1.65F, p.z}, 0.38F, Color{205, 153, 112, 255});
    DrawSphere({p.x, p.y + 1.9F, p.z}, 0.34F, accent);
    DrawCylinder({p.x - 0.18F, p.y, p.z}, 0.11F, 0.09F, 0.6F, 6, palette::Ink);
    DrawCylinder({p.x + 0.18F, p.y, p.z}, 0.11F, 0.09F, 0.6F, 6, palette::Ink);
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
