#include "app/MorningDoorCutscene.hpp"

#include "render/Palette.hpp"
#include "ui/UiDraw.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ultradope {
namespace {
constexpr float kDuration = 8.6F;
constexpr float kPi = 3.14159265F;

float Progress(float time, float start, float end) {
    return std::clamp((time - start) / (end - start), 0.0F, 1.0F);
}

float Smooth(float value) { return value * value * (3.0F - 2.0F * value); }

float Ease(float time, float start, float end) { return Smooth(Progress(time, start, end)); }

float Lerp(float from, float to, float amount) { return from + (to - from) * amount; }

Vector3 LerpVector(Vector3 from, Vector3 to, float amount) {
    return {Lerp(from.x, to.x, amount), Lerp(from.y, to.y, amount),
            Lerp(from.z, to.z, amount)};
}

Camera3D MakeCamera(Vector3 position, Vector3 target) {
    Camera3D camera{};
    camera.position = position;
    camera.target = target;
    camera.up = {0.0F, 1.0F, 0.0F};
    camera.fovy = 40.0F;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

Camera3D LerpCamera(const Camera3D& from, const Camera3D& to, float amount) {
    Camera3D camera = from;
    camera.position = LerpVector(from.position, to.position, amount);
    camera.target = LerpVector(from.target, to.target, amount);
    camera.fovy = Lerp(from.fovy, to.fovy, amount);
    return camera;
}

float TitleOpacity(float time) {
    return Ease(time, 0.7F, 1.15F) * (1.0F - Ease(time, 1.75F, 2.25F));
}

float BlackOverlayAlpha(float time) {
    if (time < 0.75F) return 1.0F - Ease(time, 0.0F, 0.75F);
    if (time < 6.85F) return 0.0F;
    if (time < 7.35F) return Ease(time, 6.85F, 7.35F);
    if (time < 7.65F) return 1.0F;
    return 1.0F - Ease(time, 7.65F, kDuration);
}
}  // namespace

void MorningDoorCutscene::Start(int day) {
    elapsed_ = 0.0F;
    day_ = std::max(1, day);
}

void MorningDoorCutscene::Advance(float deltaTime) {
    elapsed_ = std::min(kDuration, elapsed_ + std::max(0.0F, deltaTime));
}

bool MorningDoorCutscene::IsFinished() const { return elapsed_ >= kDuration; }

MorningDoorCutsceneFrame MorningDoorCutscene::Evaluate(
    const Camera3D& gameplayLandingCamera) const {
    MorningDoorCutsceneFrame frame;
    const float thresholdProgress = Ease(elapsed_, 3.0F, 3.25F);
    const float thresholdPosition = Lerp(11.72F, 11.05F, thresholdProgress);
    const float exitProgress = Ease(elapsed_, 4.0F, 6.45F);
    frame.player.position = {0.0F, Lerp(thresholdPosition, 9.0F, exitProgress)};
    frame.player.facingRadians = kPi;
    frame.player.animationTime = elapsed_;
    frame.player.walkBlend = Ease(elapsed_, 3.9F, 4.3F) *
                             (1.0F - Ease(elapsed_, 6.05F, 6.55F));
    frame.player.doorReachBlend = Ease(elapsed_, 2.05F, 2.65F) *
                                 (1.0F - Ease(elapsed_, 3.65F, 4.15F));
    frame.doorOpenFraction = Ease(elapsed_, 2.45F, 3.65F) *
                             (1.0F - Ease(elapsed_, 5.25F, 6.45F));
    frame.blackOverlayAlpha = BlackOverlayAlpha(elapsed_);

    const Camera3D establishing =
        MakeCamera({7.8F, 4.8F, 3.7F}, {0.0F, 1.8F, 12.5F});
    const Camera3D doorway =
        MakeCamera({4.25F, 2.55F, 7.7F}, {0.0F, 1.35F, 11.35F});
    const Camera3D tracking = MakeCamera(
        {4.0F, 2.65F, frame.player.position.y - 4.1F},
        {0.0F, 1.25F, frame.player.position.y - 0.15F});

    if (elapsed_ < 1.85F) {
        frame.camera = establishing;
    } else if (elapsed_ < 2.55F) {
        frame.camera = LerpCamera(establishing, doorway, Ease(elapsed_, 1.85F, 2.55F));
    } else if (elapsed_ < 4.55F) {
        frame.camera = doorway;
    } else if (elapsed_ < 5.15F) {
        frame.camera = LerpCamera(doorway, tracking, Ease(elapsed_, 4.55F, 5.15F));
    } else if (elapsed_ < 6.85F) {
        frame.camera = tracking;
    } else if (elapsed_ < 7.35F) {
        frame.camera = LerpCamera(tracking, gameplayLandingCamera,
                                  Ease(elapsed_, 6.85F, 7.35F));
    } else {
        frame.camera = gameplayLandingCamera;
    }
    return frame;
}

void MorningDoorCutscene::DrawOverlay() const {
    const float titleOpacity = TitleOpacity(elapsed_);
    if (titleOpacity > 0.0F) {
        const Rectangle panel = ui::Rect(310, 92, 660, 122);
        DrawRectangleRec(panel, ColorAlpha(Color{22, 29, 24, 255}, titleOpacity * 0.78F));
        DrawRectangleLinesEx(panel, 1.0F * ui::Scale(),
                             ColorAlpha(palette::Gold, titleOpacity * 0.62F));
        ui::CenteredText(day_ == 1 ? "FIRST LIGHT" : "A NEW MORNING",
                         ui::Rect(330, 105, 620, 50), 36,
                         ColorAlpha(palette::Gold, titleOpacity));
        char subtitle[96];
        std::snprintf(subtitle, sizeof(subtitle), "BRAMBLE ACRE  /  DAY %d  /  6:00 AM", day_);
        ui::CenteredText(subtitle, ui::Rect(330, 158, 620, 34), 18,
                         ColorAlpha(palette::Cream, titleOpacity * 0.92F));
    }

    if (elapsed_ > 0.8F && elapsed_ < 7.0F) {
        ui::CenteredText("ENTER / SPACE / ESC  -  SKIP", ui::Rect(430, 670, 420, 24), 14,
                         ColorAlpha(palette::Cream, 0.62F));
    }

    const float blackAlpha = BlackOverlayAlpha(elapsed_);
    if (blackAlpha > 0.0F) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      ColorAlpha(BLACK, blackAlpha));
    }
}

}  // namespace ultradope
