#include "ui/UiDraw.hpp"

#include "render/Palette.hpp"

#include <algorithm>

namespace ultradope::ui {

float Scale() {
    return std::max(0.75F, std::min(static_cast<float>(GetScreenWidth()) / 1280.0F,
                                    static_cast<float>(GetScreenHeight()) / 720.0F));
}

Rectangle Rect(float x, float y, float width, float height) {
    const float scale = Scale();
    return {x * scale, y * scale, width * scale, height * scale};
}

void Panel(Rectangle bounds, Color color) {
    DrawRectangleRounded(bounds, 0.14F, 8, color);
    DrawRectangleRoundedLinesEx(bounds, 0.14F, 8, 2.0F * Scale(), ColorAlpha(palette::Cream, 0.28F));
}

void Text(const char* text, float x, float y, float size, Color color) {
    DrawTextEx(GetFontDefault(), text, {x * Scale(), y * Scale()}, size * Scale(), 1.0F, color);
}

void CenteredText(const char* text, Rectangle bounds, float size, Color color) {
    const float fontSize = size * Scale();
    const Vector2 measured = MeasureTextEx(GetFontDefault(), text, fontSize, 1.0F);
    DrawTextEx(GetFontDefault(), text,
        {bounds.x + (bounds.width - measured.x) * 0.5F,
         bounds.y + (bounds.height - measured.y) * 0.5F}, fontSize, 1.0F, color);
}

bool Button(Rectangle bounds, const char* label, bool enabled) {
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), bounds);
    const Color fill = !enabled ? Color{70, 75, 70, 210} :
        (hovered ? Color{69, 126, 86, 245} : Color{48, 83, 63, 245});
    DrawRectangleRounded(bounds, 0.18F, 7, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.18F, 7, 2.0F * Scale(),
                                hovered ? palette::Gold : ColorAlpha(palette::Cream, 0.45F));
    CenteredText(label, bounds, 22.0F, enabled ? palette::Cream : GRAY);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Bar(Rectangle bounds, float fraction, Color fill, const char* label) {
    fraction = std::clamp(fraction, 0.0F, 1.0F);
    DrawRectangleRounded(bounds, 0.2F, 6, Color{25, 31, 28, 220});
    Rectangle amount = bounds;
    amount.width *= fraction;
    DrawRectangleRounded(amount, 0.2F, 6, fill);
    CenteredText(label, bounds, 16.0F, palette::Cream);
}

}  // namespace ultradope::ui
