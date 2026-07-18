#pragma once

#include <algorithm>
#include <cmath>

namespace ultradope {

struct AppPreferences {
    static constexpr float kMinCameraSensitivity = 0.5F;
    static constexpr float kMaxCameraSensitivity = 2.0F;
    static constexpr float kCameraSensitivityStep = 0.25F;

    bool invertHorizontalCamera = false;
    float cameraSensitivity = 1.0F;

    bool IsValid() const {
        return std::isfinite(cameraSensitivity) &&
               cameraSensitivity >= kMinCameraSensitivity &&
               cameraSensitivity <= kMaxCameraSensitivity;
    }

    bool CanDecreaseCameraSensitivity() const {
        return cameraSensitivity > kMinCameraSensitivity;
    }

    bool CanIncreaseCameraSensitivity() const {
        return cameraSensitivity < kMaxCameraSensitivity;
    }

    bool DecreaseCameraSensitivity() {
        if (!CanDecreaseCameraSensitivity()) return false;
        cameraSensitivity = std::max(kMinCameraSensitivity,
                                     cameraSensitivity - kCameraSensitivityStep);
        return true;
    }

    bool IncreaseCameraSensitivity() {
        if (!CanIncreaseCameraSensitivity()) return false;
        cameraSensitivity = std::min(kMaxCameraSensitivity,
                                     cameraSensitivity + kCameraSensitivityStep);
        return true;
    }

    float CameraRotationScale() const {
        const float direction = invertHorizontalCamera ? 1.0F : -1.0F;
        return direction * cameraSensitivity;
    }
};

}  // namespace ultradope
