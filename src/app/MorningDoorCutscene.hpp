#pragma once

#include "render/CharacterRenderer.hpp"

#include <raylib.h>

namespace ultradope {

struct MorningDoorCutsceneFrame {
    Camera3D camera{};
    PlayerRenderState player{};
    float doorOpenFraction = 0.0F;
    float blackOverlayAlpha = 0.0F;
};

class MorningDoorCutscene {
public:
    void Start(int day);
    void Advance(float deltaTime);
    bool IsFinished() const;
    MorningDoorCutsceneFrame Evaluate(const Camera3D& gameplayLandingCamera) const;
    void DrawOverlay() const;

private:
    float elapsed_ = 0.0F;
    int day_ = 1;
};

}  // namespace ultradope
