#pragma once

#include <raylib.h>

namespace ultradope::ui {

bool LoadGameFont();
void UnloadGameFont();
Font GameFont();
float Scale();
Rectangle Rect(float x, float y, float width, float height);
void Panel(Rectangle bounds, Color color);
void Text(const char* text, float x, float y, float size, Color color);
void CenteredText(const char* text, Rectangle bounds, float size, Color color);
bool Button(Rectangle bounds, const char* label, bool enabled = true);
void Bar(Rectangle bounds, float fraction, Color fill, const char* label);

}  // namespace ultradope::ui
