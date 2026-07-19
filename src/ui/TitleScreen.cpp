#include "ui/TitleScreen.hpp"

#include "render/Palette.hpp"
#include "ui/UiDraw.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <string>

namespace ultradope {
namespace {
Texture2D titleBackground{};
bool titleBackgroundLoaded = false;

struct TitleLayout {
    float scale;
    float offsetX;
    float offsetY;
};

TitleLayout Layout() {
    const float scale = std::min(static_cast<float>(GetScreenWidth()) / 1280.0F,
                                 static_cast<float>(GetScreenHeight()) / 720.0F);
    return {scale, (static_cast<float>(GetScreenWidth()) - 1280.0F * scale) * 0.5F,
            (static_cast<float>(GetScreenHeight()) - 720.0F * scale) * 0.5F};
}

Rectangle TitleRect(float x, float y, float width, float height) {
    const TitleLayout layout = Layout();
    return {layout.offsetX + x * layout.scale, layout.offsetY + y * layout.scale,
            width * layout.scale, height * layout.scale};
}

Vector2 TitlePoint(float x, float y) {
    const TitleLayout layout = Layout();
    return {layout.offsetX + x * layout.scale, layout.offsetY + y * layout.scale};
}

void DrawBackgroundCover() {
    if (!titleBackgroundLoaded) {
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(),
                               Color{199, 139, 39, 255}, Color{31, 35, 18, 255});
        return;
    }

    const float screenAspect = static_cast<float>(GetScreenWidth()) /
                               static_cast<float>(std::max(GetScreenHeight(), 1));
    const float textureAspect = static_cast<float>(titleBackground.width) /
                                static_cast<float>(std::max(titleBackground.height, 1));
    Rectangle source{0.0F, 0.0F, static_cast<float>(titleBackground.width),
                     static_cast<float>(titleBackground.height)};
    if (screenAspect > textureAspect) {
        const float sourceHeight = static_cast<float>(titleBackground.width) / screenAspect;
        source.y = (static_cast<float>(titleBackground.height) - sourceHeight) * 0.5F;
        source.height = sourceHeight;
    } else {
        const float sourceWidth = static_cast<float>(titleBackground.height) * screenAspect;
        source.x = (static_cast<float>(titleBackground.width) - sourceWidth) * 0.5F;
        source.width = sourceWidth;
    }
    DrawTexturePro(titleBackground, source,
                   {0.0F, 0.0F, static_cast<float>(GetScreenWidth()),
                    static_cast<float>(GetScreenHeight())},
                   {0.0F, 0.0F}, 0.0F, WHITE);
}

void DrawLogo() {
    const TitleLayout layout = Layout();
    const char* title = "POTLUCK";
    const float fontSize = 91.0F * layout.scale;
    const float spacing = -2.0F * layout.scale;
    const Vector2 measured = MeasureTextEx(ui::GameFont(), title, fontSize, spacing);
    const Vector2 origin{layout.offsetX + 46.0F * layout.scale,
                         layout.offsetY + 51.0F * layout.scale};

    // Layered imperfect registration evokes hand-cut screen-print lettering.
    DrawTextEx(ui::GameFont(), title,
               {origin.x + 12.0F * layout.scale, origin.y + 16.0F * layout.scale},
               fontSize, spacing, Color{5, 8, 3, 235});
    for (int i = 0; i < 16; ++i) {
        const float angle = static_cast<float>(i) * 3.14159265F / 8.0F;
        const Vector2 offset{std::cos(angle) * 7.0F * layout.scale,
                             std::sin(angle) * 7.0F * layout.scale};
        DrawTextEx(ui::GameFont(), title, {origin.x + offset.x, origin.y + offset.y},
                   fontSize, spacing, Color{15, 24, 8, 255});
    }
    for (int i = 0; i < 12; ++i) {
        const float angle = static_cast<float>(i) * 3.14159265F / 6.0F;
        const Vector2 offset{std::cos(angle) * 3.0F * layout.scale,
                             std::sin(angle) * 3.0F * layout.scale};
        DrawTextEx(ui::GameFont(), title, {origin.x + offset.x, origin.y + offset.y},
                   fontSize, spacing, Color{220, 196, 58, 255});
    }
    DrawTextEx(ui::GameFont(), title, origin, fontSize, spacing, Color{104, 146, 20, 255});

    const Rectangle rule{origin.x + 8.0F * layout.scale,
                         origin.y + measured.y + 3.0F * layout.scale,
                         measured.x - 16.0F * layout.scale, 4.0F * layout.scale};
}

void DrawCrack(Vector2 start, float dx, float dy, float scale, Color color) {
    const Vector2 elbow{start.x + dx * 0.55F * scale, start.y + dy * 0.34F * scale};
    const Vector2 end{start.x + dx * scale, start.y + dy * scale};
    DrawLineEx(start, elbow, 1.6F * scale, color);
    DrawLineEx(elbow, end, 1.2F * scale, color);
    DrawLineEx(elbow, {elbow.x - dy * 0.18F * scale, elbow.y + dx * 0.12F * scale},
               1.0F * scale, color);
}

void DrawStoneMenuFrame() {
    const TitleLayout layout = Layout();
    const Rectangle shadow = TitleRect(813, 164, 405, 510);
    DrawRectangleRounded({shadow.x + 12.0F * layout.scale,
                          shadow.y + 13.0F * layout.scale, shadow.width, shadow.height},
                         0.055F, 5, Color{3, 4, 2, 190});
    DrawRectangleRounded(shadow, 0.055F, 5, Color{67, 61, 46, 252});
    DrawRectangleLinesEx(shadow, 5.0F * layout.scale, Color{20, 20, 14, 255});
    DrawRectangleLinesEx(TitleRect(825, 177, 381, 486), 2.0F * layout.scale,
                         Color{133, 117, 76, 170});

    const Rectangle cap = TitleRect(834, 188, 363, 78);
    DrawRectangleRounded(cap, 0.12F, 5, Color{77, 69, 49, 255});
    DrawRectangleLinesEx(cap, 3.0F * layout.scale, Color{24, 23, 15, 255});
    DrawCircleV(TitlePoint(1015, 224), 19.0F * layout.scale, Color{29, 38, 17, 255});
    DrawLineEx(TitlePoint(1015, 240), TitlePoint(1015, 211), 3.0F * layout.scale,
               Color{111, 143, 40, 255});
    DrawEllipse(static_cast<int>(1005.0F * layout.scale + layout.offsetX),
                static_cast<int>(216.0F * layout.scale + layout.offsetY), 12.0F * layout.scale,
                6.0F * layout.scale, Color{111, 143, 40, 255});
    DrawEllipse(static_cast<int>(1025.0F * layout.scale + layout.offsetX),
                static_cast<int>(225.0F * layout.scale + layout.offsetY), 12.0F * layout.scale,
                6.0F * layout.scale, Color{111, 143, 40, 255});

    const Color crack{30, 28, 20, 210};
    DrawCrack(TitlePoint(849, 182), 27, 22, layout.scale, crack);
    DrawCrack(TitlePoint(1162, 175), -25, 27, layout.scale, crack);
    DrawCrack(TitlePoint(1187, 460), -31, 20, layout.scale, crack);
    DrawCrack(TitlePoint(838, 590), 29, -18, layout.scale, crack);
}

bool StoneButton(Rectangle bounds, const char* label, bool enabled, int index) {
    const TitleLayout layout = Layout();
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), bounds);
    const Color fill = !enabled ? Color{91, 84, 66, 245}
                               : (hovered ? Color{91, 112, 29, 255}
                                          : Color{113, 103, 78, 255});
    DrawRectangleRounded({bounds.x + 5.0F * layout.scale,
                          bounds.y + 7.0F * layout.scale, bounds.width, bounds.height},
                         0.10F, 5, Color{12, 13, 8, 210});
    DrawRectangleRounded(bounds, 0.10F, 5, fill);
    DrawRectangleLinesEx(bounds, (hovered ? 4.0F : 3.0F) * layout.scale,
                         hovered ? Color{21, 27, 9, 255} : Color{35, 32, 22, 255});
    DrawLineEx({bounds.x + 13.0F * layout.scale, bounds.y + 9.0F * layout.scale},
               {bounds.x + bounds.width - 16.0F * layout.scale,
                bounds.y + 7.0F * layout.scale},
               1.5F * layout.scale, Color{171, 151, 103, 125});
    const float crackX = bounds.x + (38.0F + static_cast<float>(index * 71 % 250)) * layout.scale;
    DrawCrack({crackX, bounds.y + 3.0F * layout.scale}, 12, 12, layout.scale,
              Color{40, 36, 25, 185});
    ui::CenteredText(label, bounds, 30,
                     enabled ? (hovered ? Color{239, 220, 133, 255}
                                        : Color{29, 34, 14, 255})
                             : Color{62, 60, 49, 180});
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
}  // namespace

bool LoadTitleScreenAssets() {
    if (titleBackgroundLoaded) return true;
    std::string path = std::string(GetApplicationDirectory()) +
                       "assets/textures/title-backyard.png";
    if (!FileExists(path.c_str())) path = "assets/textures/title-backyard.png";
    if (!FileExists(path.c_str())) return false;
    titleBackground = LoadTexture(path.c_str());
    titleBackgroundLoaded = IsTextureValid(titleBackground);
    if (titleBackgroundLoaded) SetTextureFilter(titleBackground, TEXTURE_FILTER_BILINEAR);
    return titleBackgroundLoaded;
}

void UnloadTitleScreenAssets() {
    if (!titleBackgroundLoaded) return;
    UnloadTexture(titleBackground);
    titleBackground = {};
    titleBackgroundLoaded = false;
}

void DrawTitleScreenBackdrop() {
    DrawBackgroundCover();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{84, 49, 7, 20});
    DrawRectangleGradientH(0, 0, GetScreenWidth(), GetScreenHeight(),
                           Color{5, 7, 2, 12}, Color{2, 3, 1, 58});
    for (int y = 0; y < GetScreenHeight(); y += 4) {
        DrawRectangle(0, y, GetScreenWidth(), 1, Color{8, 7, 3, 11});
    }
    DrawLogo();
    DrawStoneMenuFrame();
    ui::CenteredText("BRAMBLE ACRE  /  1972", TitleRect(0, 684, 1280, 24), 18,
                     Color{232, 215, 166, 220});
}

TitleAction DrawTitleScreen(bool hasSave) {
    DrawTitleScreenBackdrop();

    if (StoneButton(TitleRect(849, 286, 330, 69), "NEW FARM", true, 0) ||
        IsKeyPressed(KEY_N)) {
        return TitleAction::NewGame;
    }
    if (StoneButton(TitleRect(849, 369, 330, 69), "CONTINUE", hasSave, 1) ||
        (hasSave && IsKeyPressed(KEY_C))) {
        return TitleAction::Continue;
    }
    if (StoneButton(TitleRect(849, 452, 330, 69), "OPTIONS", true, 2) ||
        IsKeyPressed(KEY_O)) {
        return TitleAction::Options;
    }
    if (StoneButton(TitleRect(849, 535, 330, 69), "QUIT", true, 3) ||
        IsKeyPressed(KEY_ESCAPE)) {
        return TitleAction::Quit;
    }
    return TitleAction::None;
}

}  // namespace ultradope
