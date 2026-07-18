#include "ui/IntroScreen.hpp"

#include "render/Palette.hpp"
#include "ui/UiDraw.hpp"

#include <raylib.h>

namespace ultradope {

IntroAction DrawIntroScreen(int cardIndex) {
    static constexpr const char* kYears[] = {"1968", "1969", "1972"};
    static constexpr const char* kTitles[] = {
        "A SEED IN THE WINDOW",
        "THE FIRST LESSON",
        "ROOM TO GROW"
    };
    static constexpr const char* kLines[][3] = {
        {"You were a kid when you first noticed the plant", "hidden behind a neighbor's curtains.",
         "It looked ordinary. The secrecy did not."},
        {"Juniper taught you that good growing starts", "with patience, observation, and care.",
         "You remembered every word."},
        {"Now the seventies are opening up ahead.", "A patch of outdoor soil is yours to learn from.",
         "Someday, this little start could become an empire."}
    };

    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(),
                           Color{88, 72, 52, 255}, Color{31, 48, 39, 255});
    ui::Panel(ui::Rect(210, 95, 860, 525), Color{31, 42, 35, 245});
    ui::CenteredText(kYears[cardIndex], ui::Rect(310, 135, 660, 55), 28, palette::Gold);
    ui::CenteredText(kTitles[cardIndex], ui::Rect(280, 205, 720, 70), 40, palette::Cream);
    for (int line = 0; line < 3; ++line) {
        ui::CenteredText(kLines[cardIndex][line],
                         ui::Rect(260, 315.0F + static_cast<float>(line) * 48.0F, 760, 40),
                         21, palette::Cream);
    }
    ui::CenteredText(cardIndex == 2 ? "ENTER / CLICK - BEGIN" : "ENTER / CLICK - CONTINUE",
                     ui::Rect(390, 525, 500, 40), 17, ColorAlpha(palette::Gold, 0.9F));
    ui::Text("ESC - Skip origin", 35, 680, 15, ColorAlpha(palette::Cream, 0.65F));

    if (IsKeyPressed(KEY_ESCAPE)) return IntroAction::Skip;
    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return cardIndex == 2 ? IntroAction::Complete : IntroAction::Advance;
    }
    return IntroAction::None;
}

}  // namespace ultradope
