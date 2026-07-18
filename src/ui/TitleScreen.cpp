#include "ui/TitleScreen.hpp"

#include "render/Palette.hpp"
#include "ui/UiDraw.hpp"

#include <raylib.h>

namespace ultradope {

TitleAction DrawTitleScreen(bool hasSave) {
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(),
                           Color{81, 145, 112, 255}, Color{26, 61, 50, 255});
    for (int i = 0; i < 18; ++i) {
        const float x = static_cast<float>((i * 89) % 1280);
        const float y = 470.0F + static_cast<float>((i * 37) % 200);
        DrawCircleV({x * ui::Scale(), y * ui::Scale()}, (8.0F + static_cast<float>(i % 4) * 4.0F) * ui::Scale(),
                    ColorAlpha(palette::LeafLight, 0.35F));
    }
    ui::CenteredText("POTLUCK", ui::Rect(0, 95, 1280, 86), 86, palette::Cream);
    ui::CenteredText("Grow a legacy from soil to skyline", ui::Rect(0, 190, 1280, 24),
                     24, palette::Gold);
    ui::Panel(ui::Rect(440, 270, 400, 285), Color{24, 43, 35, 235});

    if (ui::Button(ui::Rect(490, 310, 300, 56), "NEW FARM") || IsKeyPressed(KEY_N)) {
        return TitleAction::NewGame;
    }
    if (ui::Button(ui::Rect(490, 386, 300, 56), "CONTINUE", hasSave) ||
        (hasSave && IsKeyPressed(KEY_C))) {
        return TitleAction::Continue;
    }
    if (ui::Button(ui::Rect(490, 462, 300, 56), "QUIT") || IsKeyPressed(KEY_ESCAPE)) {
        return TitleAction::Quit;
    }
    ui::CenteredText("Procedural low-poly art - no external assets", ui::Rect(0, 610, 1280, 18),
                     18, ColorAlpha(palette::Cream, 0.72F));
    return TitleAction::None;
}

}  // namespace ultradope
