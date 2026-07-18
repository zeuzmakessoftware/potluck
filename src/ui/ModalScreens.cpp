#include "ui/ModalScreens.hpp"

#include "render/Palette.hpp"
#include "systems/QuestSystem.hpp"
#include "ui/UiDraw.hpp"

#include <cstdio>

namespace ultradope {
namespace {
void ShadeWorld() { DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{10, 15, 13, 155}); }
void ModalTitle(const char* title) {
    ShadeWorld();
    ui::Panel(ui::Rect(300, 90, 680, 540), Color{28, 41, 35, 248});
    ui::CenteredText(title, ui::Rect(340, 115, 600, 55), 34, palette::Gold);
}

bool SeedButton(const GameSession& session, CropType crop, float y, bool unlocked) {
    char label[128];
    const auto& def = GetCropDefinition(crop);
    std::snprintf(label, sizeof(label), "%s seed   $%d%s", def.name, def.seedPrice,
                  unlocked ? "" : "   [LOCKED]");
    return ui::Button(ui::Rect(400, y, 480, 58), label, unlocked && session.money >= def.seedPrice);
}
}  // namespace

PauseAction DrawPauseScreen() {
    ModalTitle("PAUSED");
    if (ui::Button(ui::Rect(440, 210, 400, 55), "RESUME") || IsKeyPressed(KEY_ESCAPE)) return PauseAction::Resume;
    if (ui::Button(ui::Rect(440, 285, 400, 55), "SAVE GAME")) return PauseAction::Save;
    if (ui::Button(ui::Rect(440, 360, 400, 55), "LOAD GAME")) return PauseAction::Load;
    if (ui::Button(ui::Rect(440, 435, 400, 55), "TITLE SCREEN")) return PauseAction::Title;
    ui::Text("WASD move  arrows or mouse rotate  Wheel zoom", 395, 545, 17, palette::Cream);
    ui::Text("1-4 select  F use/place  E interact  R remove/refill", 365, 575, 17, palette::Cream);
    return PauseAction::None;
}

CropType DrawShopScreen(const GameSession& session) {
    ModalTitle("MILO'S SEED STAND");
    ui::Text("One packet plants one crop. Esc closes.", 435, 180, 18, palette::Cream);
    if (SeedButton(session, CropType::Cinderleaf, 235, true)) return CropType::Cinderleaf;
    if (SeedButton(session, CropType::VioletComet, 315, session.violetUnlocked)) return CropType::VioletComet;
    if (SeedButton(session, CropType::Sunspun, 395, session.sunspunUnlocked)) return CropType::Sunspun;
    ui::Text("Complete community quests to unlock new cultivars.", 410, 510, 18, palette::Cream);
    return CropType::None;
}

CropType DrawShippingScreen(const GameSession& session) {
    ModalTitle("OVERNIGHT SHIPPING BIN");
    const CropType crops[] = {CropType::Cinderleaf, CropType::VioletComet, CropType::Sunspun};
    for (int i = 0; i < 3; ++i) {
        const CropType crop = crops[i];
        char label[140];
        std::snprintf(label, sizeof(label), "Ship %s   owned %d   value $%d", CropName(crop),
                      session.inventory.Count(HarvestItem(crop)), GetCropDefinition(crop).salePrice);
        if (ui::Button(ui::Rect(390, 225.0F + static_cast<float>(i) * 82.0F, 500, 58), label,
                       session.inventory.Count(HarvestItem(crop)) > 0)) return crop;
    }
    ui::Text("Contents sell overnight. Esc closes.", 455, 500, 18, palette::Cream);
    return CropType::None;
}

bool DrawInventoryScreen(const GameSession& session) {
    ModalTitle("PACK");
    int index = 0;
    for (const auto& slot : session.inventory.Slots()) {
        const int column = index % 4;
        const int row = index / 4;
        const Rectangle bounds = ui::Rect(365.0F + static_cast<float>(column) * 145.0F,
                                          210.0F + static_cast<float>(row) * 105.0F, 125, 82);
        DrawRectangleRounded(bounds, 0.15F, 6, Color{48, 63, 54, 245});
        ui::CenteredText(slot.item == ItemType::None ? "EMPTY" : ItemName(slot.item),
                         {bounds.x, bounds.y + 5.0F * ui::Scale(), bounds.width, bounds.height * 0.55F},
                         14, palette::Cream);
        char count[16];
        std::snprintf(count, sizeof(count), "x%d", slot.count);
        ui::CenteredText(count, {bounds.x, bounds.y + bounds.height * 0.52F, bounds.width, bounds.height * 0.4F},
                         16, palette::Gold);
        ++index;
    }
    return IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_I);
}

bool DrawJournalScreen(const GameSession& session) {
    ModalTitle("COMMUNITY JOURNAL");
    ui::Text(QuestTitle(session.quests.active), 390, 215, 28, palette::Gold);
    ui::Text(QuestObjectiveText(session).c_str(), 390, 265, 20, palette::Cream);
    ui::Text("Outdoor Basics: learn the daily soil routine.", 390, 345, 18, palette::Cream);
    ui::Text("First Shipment: prove the harvest-to-market loop.", 390, 385, 18, palette::Cream);
    ui::Text("Hydroponics Prototype: build the first indoor system.", 390, 425, 18, palette::Cream);
    return IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_J);
}

bool DrawDialogueScreen(const GameSession&, const char* speaker, const char* text) {
    ShadeWorld();
    ui::Panel(ui::Rect(190, 455, 900, 190), Color{28, 41, 35, 248});
    ui::Text(speaker, 225, 480, 25, palette::Gold);
    ui::Text(text, 225, 530, 19, palette::Cream);
    ui::Text("E / Enter / Esc to close", 790, 600, 15, ColorAlpha(palette::Cream, 0.7F));
    return IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE);
}

bool DrawDaySummaryScreen(const DaySummary& summary) {
    ModalTitle("DAY COMPLETE");
    char line[128];
    std::snprintf(line, sizeof(line), "Day %d shipment", summary.dayEnded);
    ui::CenteredText(line, ui::Rect(390, 215, 500, 45), 25, palette::Cream);
    std::snprintf(line, sizeof(line), "%d items sold", summary.itemsSold);
    ui::CenteredText(line, ui::Rect(390, 285, 500, 45), 23, palette::Cream);
    std::snprintf(line, sizeof(line), "+$%d credits", summary.revenue);
    ui::CenteredText(line, ui::Rect(390, 340, 500, 45), 28, palette::Gold);
    ui::CenteredText(summary.note.c_str(), ui::Rect(350, 405, 580, 55), 17, palette::Cream);
    return ui::Button(ui::Rect(465, 505, 350, 58), "START NEW DAY") || IsKeyPressed(KEY_ENTER);
}

HydroUnlockAction DrawHydroUnlockedScreen() {
    ModalTitle("THE NEXT WAY TO GROW");
    ui::CenteredText("HYDROPONICS WORKSHOP UNLOCKED", ui::Rect(335, 205, 610, 70), 32,
                     palette::Gold);
    ui::CenteredText("Tessa has cleared an indoor room for a prototype.",
                     ui::Rect(350, 305, 580, 40), 19, palette::Cream);
    ui::CenteredText("Arrange the equipment, then validate flow, light, power, and cost.",
                     ui::Rect(320, 350, 640, 40), 18, palette::Cream);
    if (ui::Button(ui::Rect(350, 455, 270, 58), "ENTER WORKSHOP")) {
        return HydroUnlockAction::EnterWorkshop;
    }
    if (ui::Button(ui::Rect(660, 455, 270, 58), "STAY ON FARM")) {
        return HydroUnlockAction::StayOnFarm;
    }
    return HydroUnlockAction::None;
}

bool DrawVerticalSliceCompleteScreen() {
    ModalTitle("A NEW CHAPTER IS RUNNING");
    ui::CenteredText("HYDROPONICS PROTOTYPE ONLINE", ui::Rect(345, 210, 590, 70), 34,
                     palette::Gold);
    ui::CenteredText("The first indoor system holds steady.", ui::Rect(360, 320, 560, 40),
                     20, palette::Cream);
    ui::CenteredText("From a hidden window plant to a controlled room - this is only the start.",
                     ui::Rect(300, 370, 680, 44), 18, palette::Cream);
    return ui::Button(ui::Rect(465, 500, 350, 58), "KEEP EXPLORING") ||
           IsKeyPressed(KEY_ENTER);
}

}  // namespace ultradope
