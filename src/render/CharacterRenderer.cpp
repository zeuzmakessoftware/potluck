#include "render/CharacterRenderer.hpp"

#include "render/Palette.hpp"

#include <algorithm>
#include <cmath>
#include <rlgl.h>

namespace ultradope {
namespace {
constexpr float kPi = 3.14159265F;

struct CharacterPose {
    float bob = 0.0F;
    float sway = 0.0F;
    float leftArm = 0.0F;
    float rightArm = 0.0F;
    float leftLeg = 0.0F;
    float rightLeg = 0.0F;
    float headTilt = 0.0F;
    bool blinking = false;
};

Vector3 LocalPoint(Vector3 origin, float x, float y, float z, float facing) {
    const float sine = std::sin(facing);
    const float cosine = std::cos(facing);
    return {origin.x + x * cosine + z * sine, origin.y + y,
            origin.z - x * sine + z * cosine};
}

void DrawLimb(Vector3 start, Vector3 end, float radius, Color color) {
    const Vector3 delta{end.x - start.x, end.y - start.y, end.z - start.z};
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    DrawCylinderEx(start, end, radius, radius * 0.82F, 6, color);
    if (length > 0.0F) DrawSphere(end, radius * 1.05F, color);
}

void DrawYawCube(Vector3 position, Vector3 size, float yawDegrees, Color color) {
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(yawDegrees, 0.0F, 1.0F, 0.0F);
    DrawCubeV({0.0F, 0.0F, 0.0F}, size, color);
    rlPopMatrix();
}

void DrawPerson(Vector3 p, float facing, Color clothing, Color accent,
                const CharacterPose& pose) {
    p.y += pose.bob;
    p.x += std::cos(facing) * pose.sway;
    p.z -= std::sin(facing) * pose.sway;

    const Color denim{35, 42, 43, 255};
    const Color skin{174, 119, 78, 255};
    const float yawDegrees = facing * 180.0F / kPi;

    // Oversized head, blocky hands and chunky shoes give the cast a readable,
    // deliberately low-poly silhouette from the high gameplay camera.
    DrawCylinder(LocalPoint(p, 0.0F, 0.72F, 0.0F, facing),
                 0.38F, 0.29F, 0.92F, 6, clothing);
    DrawYawCube(LocalPoint(p, 0.0F, 0.55F, -0.02F, facing),
                {0.72F, 0.72F, 0.48F}, yawDegrees, clothing);

    const Vector3 head = LocalPoint(p, pose.headTilt, 1.64F, 0.0F, facing);
    DrawSphere(head, 0.39F, skin);
    DrawSphere(LocalPoint(p, pose.headTilt, 1.91F, -0.02F, facing), 0.35F, accent);
    DrawYawCube(LocalPoint(p, pose.headTilt, 1.87F, -0.28F, facing),
                {0.78F, 0.10F, 0.34F}, yawDegrees, accent);

    const Vector3 leftShoulder = LocalPoint(p, -0.33F, 1.10F, 0.0F, facing);
    const Vector3 rightShoulder = LocalPoint(p, 0.33F, 1.10F, 0.0F, facing);
    const Vector3 leftHand = LocalPoint(
        p, -0.49F, 0.60F + std::fabs(pose.leftArm) * 0.10F, pose.leftArm, facing);
    const Vector3 rightHand = LocalPoint(
        p, 0.49F, 0.60F + std::fabs(pose.rightArm) * 0.10F, pose.rightArm, facing);
    DrawLimb(leftShoulder, leftHand, 0.105F, clothing);
    DrawLimb(rightShoulder, rightHand, 0.105F, clothing);
    DrawSphere(leftHand, 0.13F, skin);
    DrawSphere(rightHand, 0.13F, skin);

    const Vector3 leftHip = LocalPoint(p, -0.18F, 0.35F, 0.0F, facing);
    const Vector3 rightHip = LocalPoint(p, 0.18F, 0.35F, 0.0F, facing);
    const Vector3 leftFoot = LocalPoint(
        p, -0.20F, -0.05F + std::max(0.0F, -pose.leftLeg) * 0.12F,
        pose.leftLeg, facing);
    const Vector3 rightFoot = LocalPoint(
        p, 0.20F, -0.05F + std::max(0.0F, -pose.rightLeg) * 0.12F,
        pose.rightLeg, facing);
    DrawLimb(leftHip, leftFoot, 0.13F, denim);
    DrawLimb(rightHip, rightFoot, 0.13F, denim);
    DrawYawCube(LocalPoint(leftFoot, 0.0F, -0.03F, -0.10F, facing),
                {0.34F, 0.16F, 0.55F}, yawDegrees, palette::Ink);
    DrawYawCube(LocalPoint(rightFoot, 0.0F, -0.03F, -0.10F, facing),
                {0.34F, 0.16F, 0.55F}, yawDegrees, palette::Ink);

    // A blink briefly squashes both eyes instead of popping them out of existence.
    const float eyeRadius = pose.blinking ? 0.025F : 0.055F;
    const float eyeHeight = pose.blinking ? 0.018F : 0.065F;
    const Vector3 leftEye = LocalPoint(p, pose.headTilt - 0.13F, 1.69F, 0.35F, facing);
    const Vector3 rightEye = LocalPoint(p, pose.headTilt + 0.13F, 1.69F, 0.35F, facing);
    DrawCylinderEx({leftEye.x, leftEye.y - eyeHeight * 0.5F, leftEye.z},
                   {leftEye.x, leftEye.y + eyeHeight * 0.5F, leftEye.z},
                   eyeRadius, eyeRadius, 6, palette::Ink);
    DrawCylinderEx({rightEye.x, rightEye.y - eyeHeight * 0.5F, rightEye.z},
                   {rightEye.x, rightEye.y + eyeHeight * 0.5F, rightEye.z},
                   eyeRadius, eyeRadius, 6, palette::Ink);

    DrawCircle3D({p.x, 0.015F, p.z}, 0.48F, {1.0F, 0.0F, 0.0F}, 90.0F,
                 Color{30, 35, 28, 75});
}

float NamePhase(const char* name) {
    unsigned int value = 0U;
    for (const char* letter = name; letter != nullptr && *letter != '\0'; ++letter) {
        value = value * 33U + static_cast<unsigned int>(*letter);
    }
    return static_cast<float>(value % 628U) / 100.0F;
}
}  // namespace

void DrawPlayerCharacter(const PlayerState& player, float animationTime, bool moving) {
    CharacterPose pose;
    if (moving) {
        const float stride = std::sin(animationTime * 9.0F);
        pose.bob = std::fabs(std::cos(animationTime * 9.0F)) * 0.065F;
        pose.sway = stride * 0.025F;
        pose.leftArm = stride * 0.34F;
        pose.rightArm = -stride * 0.34F;
        pose.leftLeg = -stride * 0.25F;
        pose.rightLeg = stride * 0.25F;
        pose.headTilt = -stride * 0.018F;
    } else {
        const float breath = std::sin(animationTime * 2.2F);
        pose.bob = breath * 0.018F;
        pose.sway = std::sin(animationTime * 1.1F) * 0.012F;
        pose.leftArm = breath * 0.025F;
        pose.rightArm = -breath * 0.025F;
        pose.headTilt = std::sin(animationTime * 0.8F) * 0.015F;
    }
    pose.blinking = std::fmod(animationTime, 4.2F) > 4.05F;

    DrawPerson({player.position.x, 0.58F, player.position.y}, player.facingRadians,
               Color{62, 107, 145, 255}, palette::Gold, pose);
}

void DrawNpcCharacter(Vector3 position, Color clothing, Color accent, const char* name,
                      float animationTime) {
    const float phase = NamePhase(name);
    const float time = animationTime + phase;
    CharacterPose pose;
    pose.bob = std::sin(time * 1.8F) * 0.016F;
    pose.sway = std::sin(time * 0.75F) * 0.025F;
    pose.leftArm = std::sin(time * 1.15F) * 0.045F;
    pose.rightArm = -pose.leftArm;
    pose.headTilt = std::sin(time * 0.65F) * 0.025F;
    pose.blinking = std::fmod(time, 3.7F) > 3.55F;

    // Each NPC periodically makes a small greeting gesture at a different time.
    const float gesture = std::max(0.0F, std::sin(time * 0.72F) - 0.72F) / 0.28F;
    pose.rightArm += gesture * (0.74F + std::sin(time * 5.0F) * 0.09F);
    pose.headTilt -= gesture * 0.035F;

    // Face the busy center of the farm instead of lining every NPC up identically.
    const float facing = std::atan2(-position.x, -position.z);
    DrawPerson({position.x, 0.58F, position.z}, facing, clothing, accent, pose);
}

}  // namespace ultradope
