#include "camera/ThirdPersonCamera.hpp"

#include <algorithm>
#include <cmath>

namespace ultradope {

ThirdPersonCamera::ThirdPersonCamera() {
    camera_.up = {0.0F, 1.0F, 0.0F};
    camera_.fovy = 45.0F;
    camera_.projection = CAMERA_PERSPECTIVE;
    Reset(CameraProfile::Outdoor);
}

void ThirdPersonCamera::Reset(CameraProfile profile) {
    yaw_ = 0.0F;
    if (profile == CameraProfile::Indoor) {
        distance_ = 9.5F;
        minDistance_ = 6.0F;
        maxDistance_ = 13.0F;
        heightFactor_ = 0.48F;
    } else {
        distance_ = 14.0F;
        minDistance_ = 8.0F;
        maxDistance_ = 20.0F;
        heightFactor_ = 0.62F;
    }
    camera_.target = {0.0F, 1.1F, 0.0F};
    camera_.position = {0.0F, 1.1F + distance_ * heightFactor_, distance_};
}

void ThirdPersonCamera::Update(const PlayerState& player, float rotationRate,
                               float rotationDelta, float zoomInput, float deltaTime) {
    yaw_ += rotationRate * deltaTime * 1.8F + rotationDelta;
    distance_ = std::clamp(distance_ - zoomInput * 1.2F, minDistance_, maxDistance_);
    const Vector3 target = {player.position.x, 1.1F, player.position.y};
    const Vector3 desired = {
        target.x + std::sin(yaw_) * distance_,
        target.y + distance_ * heightFactor_,
        target.z + std::cos(yaw_) * distance_
    };
    const float blend = 1.0F - std::exp(-deltaTime * 8.0F);
    camera_.target.x += (target.x - camera_.target.x) * blend;
    camera_.target.y += (target.y - camera_.target.y) * blend;
    camera_.target.z += (target.z - camera_.target.z) * blend;
    camera_.position.x += (desired.x - camera_.position.x) * blend;
    camera_.position.y += (desired.y - camera_.position.y) * blend;
    camera_.position.z += (desired.z - camera_.position.z) * blend;
}

Vec2 ThirdPersonCamera::MovementVector(float right, float forward) const {
    Vec2 direction;
    direction.x = std::cos(yaw_) * right - std::sin(yaw_) * forward;
    direction.y = -std::sin(yaw_) * right - std::cos(yaw_) * forward;
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 1.0F) {
        direction.x /= length;
        direction.y /= length;
    }
    return direction;
}

const Camera3D& ThirdPersonCamera::Camera() const { return camera_; }

}  // namespace ultradope
