#include "ui/IntroScreen.hpp"

#include "render/Palette.hpp"
#include "ui/UiDraw.hpp"

#include <raylib.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>

namespace ultradope {
namespace {
constexpr int kCardCount = 3;
constexpr float kDesignWidth = 1280.0F;
constexpr float kDesignHeight = 720.0F;

struct OriginCard {
    const char* year;
    const char* title;
    std::array<const char*, 3> lines;
    const char* texturePath;
    float startZoom;
    float endZoom;
    float panX;
    float panY;
};

constexpr std::array<OriginCard, kCardCount> kCards{{
    {"1968", "A SEED IN THE WINDOW",
     {"You were a kid when you first noticed the plant",
      "hidden behind a neighbor's curtains.",
      "It looked ordinary. The secrecy did not."},
     "assets/textures/origin-1968-window.png", 1.055F, 1.105F, 0.78F, -0.12F},
    {"1969", "THE FIRST LESSON",
     {"Juniper taught you that good growing starts",
      "with patience, observation, and care.",
      "You remembered every word."},
     "assets/textures/origin-1969-lesson.png", 1.075F, 1.105F, 0.52F, 0.10F},
    {"1972", "ROOM TO GROW",
     {"Now the seventies are opening up ahead.",
      "A patch of outdoor soil is yours to learn from.",
      "Someday, this little start could become an empire."},
     "assets/textures/origin-1972-acre.png", 1.12F, 1.065F, 0.38F, -0.35F}
}};

std::array<Texture2D, kCardCount> cardTextures{};
std::array<bool, kCardCount> cardTexturesLoaded{};

struct IntroLayout {
    float scale;
    float offsetX;
    float offsetY;
};

IntroLayout Layout() {
    const float scale = std::min(static_cast<float>(GetScreenWidth()) / kDesignWidth,
                                 static_cast<float>(GetScreenHeight()) / kDesignHeight);
    return {scale, (static_cast<float>(GetScreenWidth()) - kDesignWidth * scale) * 0.5F,
            (static_cast<float>(GetScreenHeight()) - kDesignHeight * scale) * 0.5F};
}

Rectangle IntroRect(float x, float y, float width, float height) {
    const IntroLayout layout = Layout();
    return {layout.offsetX + x * layout.scale, layout.offsetY + y * layout.scale,
            width * layout.scale, height * layout.scale};
}

Vector2 IntroPoint(float x, float y) {
    const IntroLayout layout = Layout();
    return {layout.offsetX + x * layout.scale, layout.offsetY + y * layout.scale};
}

float SmoothStep(float value) {
    value = std::clamp(value, 0.0F, 1.0F);
    return value * value * (3.0F - 2.0F * value);
}

float Reveal(float cardTime, float start, float duration) {
    return SmoothStep((cardTime - start) / duration);
}

Color FadeColor(Color color, float opacity) {
    return ColorAlpha(color, std::clamp(opacity, 0.0F, 1.0F));
}

void DrawIntroText(const char* text, float x, float y, float size, Color color) {
    const IntroLayout layout = Layout();
    DrawTextEx(ui::GameFont(), text, IntroPoint(x, y), size * layout.scale,
               1.0F * layout.scale, color);
}

void DrawRevealedText(const char* text, float x, float y, float size, Color color,
                      float amount, float rise = 9.0F) {
    if (amount <= 0.0F) return;
    const float drawY = y + (1.0F - amount) * rise;
    DrawIntroText(text, x + 2.0F, drawY + 3.0F, size,
                  FadeColor(Color{3, 5, 3, 230}, amount));
    DrawIntroText(text, x, drawY, size, FadeColor(color, amount));
}

void DrawProceduralFallback(int cardIndex, float animationTime) {
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();
    DrawRectangleGradientV(0, 0, width, height, Color{54, 54, 30, 255},
                           Color{13, 20, 14, 255});

    if (cardIndex == 0) {
        DrawRectangleRec(IntroRect(720, 72, 500, 560), Color{30, 29, 21, 255});
        DrawRectangleRec(IntroRect(848, 142, 280, 310), Color{207, 154, 68, 255});
        DrawRectangleRec(IntroRect(866, 160, 244, 274), Color{226, 185, 100, 255});
        DrawRectangleRec(IntroRect(981, 160, 10, 274), Color{45, 37, 25, 255});
        DrawRectangleRec(IntroRect(866, 292, 244, 10), Color{45, 37, 25, 255});
        const float curtain = std::sin(animationTime * 0.38F) * 10.0F;
        DrawTriangle(IntroPoint(866 + curtain, 160), IntroPoint(942 + curtain, 160),
                     IntroPoint(866 + curtain, 430), Color{104, 72, 35, 210});
        DrawTriangle(IntroPoint(1110 - curtain, 160), IntroPoint(1034 - curtain, 160),
                     IntroPoint(1110 - curtain, 430), Color{104, 72, 35, 210});
        DrawRectangleRec(IntroRect(974, 366, 48, 40), Color{79, 43, 26, 255});
        DrawLineEx(IntroPoint(998, 370), IntroPoint(998, 312), 6.0F * Layout().scale,
                   Color{36, 55, 24, 255});
        DrawCircleV(IntroPoint(976, 327), 21.0F * Layout().scale, Color{50, 78, 34, 255});
        DrawCircleV(IntroPoint(1018, 322), 23.0F * Layout().scale, Color{50, 78, 34, 255});
        DrawCircleV(IntroPoint(758, 552), 42.0F * Layout().scale, Color{8, 11, 8, 255});
        DrawRectangleRec(IntroRect(720, 590, 76, 120), Color{8, 11, 8, 255});
    } else if (cardIndex == 1) {
        DrawRectangleRec(IntroRect(716, 448, 520, 110), Color{69, 43, 25, 255});
        DrawRectangleRec(IntroRect(944, 130, 176, 28), Color{57, 37, 21, 255});
        DrawTriangle(IntroPoint(965, 158), IntroPoint(1098, 158), IntroPoint(1066, 225),
                     Color{112, 69, 31, 255});
        const float pulse = 0.84F + std::sin(animationTime * 1.1F) * 0.05F;
        DrawCircleGradient(static_cast<int>(1030.0F * Layout().scale + Layout().offsetX),
                           static_cast<int>(236.0F * Layout().scale + Layout().offsetY),
                           150.0F * Layout().scale, FadeColor(Color{225, 174, 75, 130}, pulse),
                           Color{225, 174, 75, 0});
        DrawCircleV(IntroPoint(835, 340), 58.0F * Layout().scale, Color{15, 17, 12, 255});
        DrawTriangle(IntroPoint(768, 548), IntroPoint(892, 548), IntroPoint(845, 388),
                     Color{21, 23, 17, 255});
        DrawCircleV(IntroPoint(1150, 330), 62.0F * Layout().scale, Color{14, 16, 11, 255});
        DrawTriangle(IntroPoint(1074, 548), IntroPoint(1224, 548), IntroPoint(1156, 390),
                     Color{20, 22, 16, 255});
        DrawRectangleRec(IntroRect(1004, 413, 55, 49), Color{82, 48, 28, 255});
        DrawLineEx(IntroPoint(1032, 418), IntroPoint(1032, 365), 5.0F * Layout().scale,
                   Color{45, 72, 32, 255});
        DrawCircleV(IntroPoint(1012, 379), 18.0F * Layout().scale, Color{65, 91, 38, 255});
        DrawCircleV(IntroPoint(1051, 375), 19.0F * Layout().scale, Color{65, 91, 38, 255});
    } else {
        DrawCircleGradient(static_cast<int>(1080.0F * Layout().scale + Layout().offsetX),
                           static_cast<int>(170.0F * Layout().scale + Layout().offsetY),
                           125.0F * Layout().scale, Color{231, 185, 84, 185},
                           Color{231, 185, 84, 0});
        DrawRectangleRec(IntroRect(978, 296, 235, 225), Color{184, 157, 104, 255});
        DrawTriangle(IntroPoint(950, 300), IntroPoint(1095, 218), IntroPoint(1240, 300),
                     Color{87, 49, 29, 255});
        DrawRectangleRec(IntroRect(1080, 414, 55, 107), Color{46, 39, 27, 255});
        for (int row = 0; row < 7; ++row) {
            const float y = 720.0F - static_cast<float>(row) * 55.0F;
            DrawLineEx(IntroPoint(700, y),
                       IntroPoint(1110, 500.0F - static_cast<float>(row) * 24.0F),
                       3.0F * Layout().scale, Color{96, 74, 38, 180});
        }
        DrawRectangleRec(IntroRect(740, 420, 18, 300), Color{73, 42, 25, 255});
        DrawRectangleRec(IntroRect(1120, 380, 18, 340), Color{73, 42, 25, 255});
        DrawLineEx(IntroPoint(748, 452), IntroPoint(1129, 414), 16.0F * Layout().scale,
                   Color{91, 53, 30, 255});
        DrawCircleV(IntroPoint(884, 335), 49.0F * Layout().scale, Color{12, 15, 10, 255});
        DrawTriangle(IntroPoint(817, 552), IntroPoint(940, 552), IntroPoint(886, 378),
                     Color{16, 19, 13, 255});
    }
}

void DrawBackgroundCover(int cardIndex, float cardTime, float animationTime) {
    const std::size_t index = static_cast<std::size_t>(cardIndex);
    if (!cardTexturesLoaded[index]) {
        DrawProceduralFallback(cardIndex, animationTime);
        return;
    }

    const Texture2D texture = cardTextures[index];
    const float screenAspect = static_cast<float>(GetScreenWidth()) /
                               static_cast<float>(std::max(GetScreenHeight(), 1));
    const float textureAspect = static_cast<float>(texture.width) /
                                static_cast<float>(std::max(texture.height, 1));
    Rectangle baseSource{0.0F, 0.0F, static_cast<float>(texture.width),
                         static_cast<float>(texture.height)};
    if (screenAspect > textureAspect) {
        const float sourceHeight = static_cast<float>(texture.width) / screenAspect;
        baseSource.y = (static_cast<float>(texture.height) - sourceHeight) * 0.5F;
        baseSource.height = sourceHeight;
    } else {
        const float sourceWidth = static_cast<float>(texture.height) * screenAspect;
        baseSource.x = (static_cast<float>(texture.width) - sourceWidth) * 0.5F;
        baseSource.width = sourceWidth;
    }

    const OriginCard& card = kCards[index];
    const float travel = SmoothStep(std::min(cardTime / 12.0F, 1.0F));
    const float zoom = card.startZoom + (card.endZoom - card.startZoom) * travel;
    Rectangle source{baseSource.x, baseSource.y, baseSource.width / zoom,
                     baseSource.height / zoom};
    const float insetX = (baseSource.width - source.width) * 0.5F;
    const float insetY = (baseSource.height - source.height) * 0.5F;
    const float driftX = card.panX * insetX * (-0.30F + travel * 1.15F);
    const float driftY = card.panY * insetY * (-0.20F + travel);
    const float gateX = static_cast<float>((static_cast<int>(animationTime * 10.0F) % 3) - 1);
    const float gateY = static_cast<float>((static_cast<int>(animationTime * 8.0F + 1.0F) % 3) - 1);
    source.x += insetX + driftX + gateX;
    source.y += insetY + driftY + gateY;
    source.x = std::clamp(source.x, 0.0F, static_cast<float>(texture.width) - source.width);
    source.y = std::clamp(source.y, 0.0F, static_cast<float>(texture.height) - source.height);

    DrawTexturePro(texture, source,
                   {0.0F, 0.0F, static_cast<float>(GetScreenWidth()),
                    static_cast<float>(GetScreenHeight())},
                   {0.0F, 0.0F}, 0.0F, WHITE);
}

void DrawSceneAccent(int cardIndex, float animationTime) {
    const IntroLayout layout = Layout();
    if (cardIndex == 0) {
        const float drift = std::sin(animationTime * 0.25F) * 10.0F;
        for (int band = 0; band < 3; ++band) {
            Rectangle shadow = IntroRect(802.0F + drift,
                                         218.0F + static_cast<float>(band) * 78.0F,
                                         430.0F, 20.0F);
            DrawRectangleRec(shadow, Color{24, 17, 8, static_cast<unsigned char>(18 + band * 4)});
        }
    } else if (cardIndex == 1) {
        const int dustFrame = static_cast<int>(animationTime * 9.0F);
        for (int i = 0; i < 15; ++i) {
            const float x = 760.0F + static_cast<float>((i * 71 + dustFrame * 5) % 440);
            const float y = 165.0F + static_cast<float>((i * 43 + dustFrame * 3) % 360);
            DrawCircleV(IntroPoint(x, y), (1.0F + static_cast<float>(i % 2)) * layout.scale,
                        Color{233, 199, 113, static_cast<unsigned char>(28 + i % 3 * 8)});
        }
    } else {
        const float sway = std::sin(animationTime * 0.65F) * 9.0F;
        DrawRectangleGradientH(0, static_cast<int>(IntroPoint(0, 470).y), GetScreenWidth(),
                               static_cast<int>(210.0F * layout.scale),
                               Color{187, 155, 79, 5}, Color{218, 181, 103, 27});
        for (int i = 0; i < 7; ++i) {
            const float blade = static_cast<float>(i);
            const float heightStep = static_cast<float>(i % 3);
            const Vector2 root = IntroPoint(800.0F + blade * 64.0F, 720.0F);
            DrawLineEx(root, IntroPoint(805.0F + blade * 64.0F + sway,
                                        625.0F - heightStep * 13.0F),
                       3.0F * layout.scale, Color{36, 48, 23, 100});
        }
    }
}

void DrawAnalogOverlay(float animationTime) {
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();
    const unsigned char warmth = static_cast<unsigned char>(17 +
        static_cast<int>((std::sin(animationTime * 0.29F) + 1.0F) * 2.5F));
    DrawRectangle(0, 0, width, height, Color{92, 50, 8, warmth});
    DrawRectangleGradientH(0, 0, width, height, Color{2, 5, 2, 36}, Color{7, 5, 2, 8});

    const float leakX = std::fmod(animationTime * 42.0F, static_cast<float>(width + 520)) - 260.0F;
    DrawRectangleGradientH(static_cast<int>(leakX), 0, 260, height,
                           Color{228, 157, 62, 0}, Color{228, 157, 62, 13});
    DrawRectangleGradientH(static_cast<int>(leakX + 260.0F), 0, 260, height,
                           Color{228, 157, 62, 13}, Color{228, 157, 62, 0});

    const int scanlinePhase = static_cast<int>(animationTime * 2.0F) % 4;
    for (int y = scanlinePhase; y < height; y += 4) {
        DrawRectangle(0, y, width, 1, Color{7, 6, 3, 12});
    }
    for (int band = 0; band < 9; ++band) {
        const int inset = band * 4;
        const unsigned char alpha = static_cast<unsigned char>(8 + (9 - band) * 2);
        DrawRectangle(inset, inset, width - inset * 2, 3, Color{0, 0, 0, alpha});
        DrawRectangle(inset, height - inset - 3, width - inset * 2, 3,
                      Color{0, 0, 0, alpha});
        DrawRectangle(inset, inset, 3, height - inset * 2, Color{0, 0, 0, alpha});
        DrawRectangle(width - inset - 3, inset, 3, height - inset * 2,
                      Color{0, 0, 0, alpha});
    }
    const int grainFrame = static_cast<int>(animationTime * 12.0F);
    for (int i = 0; i < 34; ++i) {
        const int x = (i * 211 + grainFrame * 31) % std::max(width, 1);
        const int y = (i * 89 + grainFrame * 19) % std::max(height, 1);
        DrawRectangle(x, y, 1 + (i % 2), 1, Color{235, 204, 139, 19});
    }
}

void DrawStoryPanel() {
    const IntroLayout layout = Layout();
    const Rectangle shadow = IntroRect(42, 60, 610, 600);
    DrawRectangleRec({shadow.x + 10.0F * layout.scale, shadow.y + 12.0F * layout.scale,
                      shadow.width, shadow.height}, Color{0, 0, 0, 125});
    DrawRectangleGradientH(static_cast<int>(shadow.x), static_cast<int>(shadow.y),
                           static_cast<int>(shadow.width), static_cast<int>(shadow.height),
                           Color{20, 27, 20, 249}, Color{25, 31, 22, 222});
    DrawRectangleLinesEx(shadow, 2.0F * layout.scale, ColorAlpha(palette::Gold, 0.58F));
    const Rectangle inset = IntroRect(51, 69, 590, 580);
    DrawRectangleLinesEx(inset, 1.0F * layout.scale, ColorAlpha(palette::Cream, 0.13F));
    DrawLineEx(IntroPoint(42, 77), IntroPoint(59, 60), 3.0F * layout.scale, palette::Ink);
    DrawLineEx(IntroPoint(635, 660), IntroPoint(652, 643), 3.0F * layout.scale, palette::Ink);
    for (int notch = 0; notch < 5; ++notch) {
        const float y = 132.0F + static_cast<float>(notch) * 103.0F;
        DrawTriangle(IntroPoint(652, y), IntroPoint(652, y + 19.0F),
                     IntroPoint(644, y + 10.0F), Color{16, 21, 16, 225});
    }
}

void DrawCardCopy(int cardIndex, float cardTime, float animationTime) {
    const std::size_t index = static_cast<std::size_t>(cardIndex);
    const OriginCard& card = kCards[index];
    const float yearReveal = Reveal(cardTime, 0.08F, 0.28F);
    const float titleReveal = Reveal(cardTime, 0.22F, 0.38F);
    const float lineOneReveal = Reveal(cardTime, 0.50F, 0.30F);
    const float lineTwoReveal = Reveal(cardTime, 0.66F, 0.30F);
    const float lineThreeReveal = Reveal(cardTime, 0.82F, 0.30F);

    DrawRevealedText(card.year, 88, 94, 27, palette::Gold, yearReveal, 7.0F);
    DrawRectangleRec(IntroRect(88, 143, 118.0F * yearReveal, 4),
                     FadeColor(palette::Gold, yearReveal * 0.8F));
    DrawRevealedText(card.title, 88, 172, 34, palette::Cream, titleReveal, 10.0F);
    DrawRevealedText(card.lines[0], 88, 286, 19, palette::Cream, lineOneReveal);
    DrawRevealedText(card.lines[1], 88, 332, 19, palette::Cream, lineTwoReveal);
    DrawRevealedText(card.lines[2], 88, 378, 19, palette::Cream, lineThreeReveal);

    const float progressReveal = Reveal(cardTime, 0.72F, 0.35F);
    for (int i = 0; i < kCardCount; ++i) {
        const Rectangle marker = IntroRect(88.0F + static_cast<float>(i) * 39.0F,
                                           500, 27, 7);
        DrawRectangleRec(marker, FadeColor(i <= cardIndex ? palette::Gold : palette::Cream,
                                           progressReveal * (i <= cardIndex ? 0.88F : 0.22F)));
    }

    const float promptReveal = Reveal(cardTime, 0.95F, 0.30F);
    const float pulse = 0.73F + (std::sin(animationTime * 3.1F) + 1.0F) * 0.11F;
    DrawRevealedText(cardIndex == kCardCount - 1 ? "ENTER / CLICK - BEGIN" :
                                                       "ENTER / CLICK - CONTINUE",
                     88, 548, 17, palette::Gold, promptReveal * pulse, 5.0F);
    DrawRevealedText("ESC - Skip origin", 88, 612, 14, palette::Cream,
                     Reveal(cardTime, 0.45F, 0.3F) * 0.58F, 4.0F);
}
}  // namespace

bool LoadIntroScreenAssets() {
    bool allLoaded = true;
    for (std::size_t i = 0; i < kCards.size(); ++i) {
        if (cardTexturesLoaded[i]) continue;
        std::string path = std::string(GetApplicationDirectory()) + kCards[i].texturePath;
        if (!FileExists(path.c_str())) path = kCards[i].texturePath;
        if (!FileExists(path.c_str())) {
            allLoaded = false;
            continue;
        }
        cardTextures[i] = LoadTexture(path.c_str());
        cardTexturesLoaded[i] = IsTextureValid(cardTextures[i]);
        if (cardTexturesLoaded[i]) {
            SetTextureFilter(cardTextures[i], TEXTURE_FILTER_BILINEAR);
        } else {
            cardTextures[i] = {};
            allLoaded = false;
        }
    }
    return allLoaded;
}

void UnloadIntroScreenAssets() {
    for (std::size_t i = 0; i < kCards.size(); ++i) {
        if (cardTexturesLoaded[i]) UnloadTexture(cardTextures[i]);
        cardTextures[i] = {};
        cardTexturesLoaded[i] = false;
    }
}

IntroAction DrawIntroScreen(const IntroPresentation& presentation) {
    const int cardIndex = std::clamp(presentation.cardIndex, 0, kCardCount - 1);
    DrawBackgroundCover(cardIndex, presentation.cardTime, presentation.animationTime);
    DrawSceneAccent(cardIndex, presentation.animationTime);
    DrawAnalogOverlay(presentation.animationTime);
    DrawStoryPanel();
    DrawCardCopy(cardIndex, presentation.cardTime, presentation.animationTime);

    const float fade = SmoothStep(presentation.fadeOpacity);
    if (fade > 0.0F) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      Color{4, 5, 3, static_cast<unsigned char>(fade * 255.0F)});
        const float splice = 1.0F - std::fabs(fade - 0.52F) / 0.08F;
        if (splice > 0.0F) {
            DrawRectangle(0, GetScreenHeight() / 2, GetScreenWidth(), 2,
                          FadeColor(Color{233, 208, 139, 90}, splice));
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) return IntroAction::Skip;
    if (presentation.inputEnabled &&
        (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        return cardIndex == kCardCount - 1 ? IntroAction::Complete : IntroAction::Advance;
    }
    return IntroAction::None;
}

}  // namespace ultradope
