#pragma once

#include "domain/GameSession.hpp"

#include <raylib.h>

namespace ultradope {

enum class CameraProfile { Outdoor, Indoor };

class ThirdPersonCamera {
public:
    ThirdPersonCamera();

    void Reset(CameraProfile profile);
    void Update(const PlayerState& player, float rotationRate, float rotationDelta,
                float zoomInput, float deltaTime);
    Vec2 MovementVector(float right, float forward) const;
    const Camera3D& Camera() const;

private:
    Camera3D camera_{};
    float yaw_ = 0.0F;
    float distance_ = 14.0F;
    float minDistance_ = 8.0F;
    float maxDistance_ = 20.0F;
    float heightFactor_ = 0.62F;
};

}  // namespace ultradope
