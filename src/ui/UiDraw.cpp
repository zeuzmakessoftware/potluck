#include "ui/UiDraw.hpp"

#include "render/Palette.hpp"

#include <algorithm>
#include <string>

namespace ultradope::ui {
namespace {
Font gameFont{};
bool gameFontLoaded = false;

std::string BundledFontPath() {
    return std::string(GetApplicationDirectory()) + "assets/fonts/UnifrakturCook-Bold.ttf";
}
}  // namespace

bool LoadGameFont() {
    if (gameFontLoaded) return true;
    std::string path = BundledFontPath();
    if (!FileExists(path.c_str())) path = "assets/fonts/UnifrakturCook-Bold.ttf";
    if (!FileExists(path.c_str())) return false;

    gameFont = LoadFontEx(path.c_str(), 96, nullptr, 0);
    gameFontLoaded = IsFontValid(gameFont);
    if (gameFontLoaded) SetTextureFilter(gameFont.texture, TEXTURE_FILTER_BILINEAR);
    return gameFontLoaded;
}

void UnloadGameFont() {
    if (!gameFontLoaded) return;
    UnloadFont(gameFont);
    gameFont = {};
    gameFontLoaded = false;
}

Font GameFont() { return gameFontLoaded ? gameFont : GetFontDefault(); }

float Scale() {
    return std::max(0.75F, std::min(static_cast<float>(GetScreenWidth()) / 1280.0F,
                                    static_cast<float>(GetScreenHeight()) / 720.0F));
}

Rectangle Rect(float x, float y, float width, float height) {
    const float scale = Scale();
    return {x * scale, y * scale, width * scale, height * scale};
}

void Panel(Rectangle bounds, Color color) {
    const float s = Scale();
    DrawRectangleRec({bounds.x + 4.0F * s, bounds.y + 5.0F * s, bounds.width, bounds.height},
                     Color{0, 0, 0, 105});
    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 1.0F * s, ColorAlpha(palette::Gold, 0.52F));
    DrawRectangleLinesEx({bounds.x + 4.0F * s, bounds.y + 4.0F * s,
                          bounds.width - 8.0F * s, bounds.height - 8.0F * s},
                         1.0F * s, ColorAlpha(palette::Cream, 0.12F));
    // Clipped corner marks make otherwise clean procedural UI feel screen-printed.
    DrawLineEx({bounds.x, bounds.y + 11.0F * s}, {bounds.x + 11.0F * s, bounds.y},
               2.0F * s, palette::Ink);
    DrawLineEx({bounds.x + bounds.width - 11.0F * s, bounds.y + bounds.height},
               {bounds.x + bounds.width, bounds.y + bounds.height - 11.0F * s},
               2.0F * s, palette::Ink);
}

void Text(const char* text, float x, float y, float size, Color color) {
    DrawTextEx(GameFont(), text, {x * Scale(), y * Scale()}, size * Scale(), 1.0F, color);
}

void CenteredText(const char* text, Rectangle bounds, float size, Color color) {
    const float fontSize = size * Scale();
    const Vector2 measured = MeasureTextEx(GameFont(), text, fontSize, 1.0F);
    DrawTextEx(GameFont(), text,
        {bounds.x + (bounds.width - measured.x) * 0.5F,
         bounds.y + (bounds.height - measured.y) * 0.5F}, fontSize, 1.0F, color);
}

bool Button(Rectangle bounds, const char* label, bool enabled) {
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), bounds);
    const Color fill = !enabled ? Color{70, 75, 70, 210} :
        (hovered ? Color{69, 126, 86, 245} : Color{48, 83, 63, 245});
    DrawRectangleRec(bounds, fill);
    DrawRectangleLinesEx(bounds, 2.0F * Scale(),
                         hovered ? palette::Gold : ColorAlpha(palette::Cream, 0.45F));
    CenteredText(label, bounds, 22.0F, enabled ? palette::Cream : GRAY);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Bar(Rectangle bounds, float fraction, Color fill, const char* label) {
    fraction = std::clamp(fraction, 0.0F, 1.0F);
    DrawRectangleRec(bounds, Color{8, 10, 8, 235});
    Rectangle amount = bounds;
    amount.width *= fraction;
    DrawRectangleRec(amount, fill);
    DrawRectangleLinesEx(bounds, 1.0F * Scale(), ColorAlpha(palette::Gold, 0.55F));
    CenteredText(label, bounds, 16.0F, palette::Cream);
}

}  // namespace ultradope::ui
