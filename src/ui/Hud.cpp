#include "ui/Hud.hpp"

#include "render/Palette.hpp"
#include "systems/QuestSystem.hpp"
#include "ui/UiDraw.hpp"

#include <cstdio>

namespace ultradope {
namespace {
const char* ToolName(const GameSession& session, int index) {
    switch (index) {
        case 0: return "1  HOE";
        case 1: return "2  WATER";
        case 2: return "3  HAND";
        case 3: return CropName(session.selectedCrop);
        default: return "";
    }
}

const char* EquipmentName(HydroEquipment equipment) {
    switch (equipment) {
        case HydroEquipment::Reservoir: return "RESERVOIR";
        case HydroEquipment::Pump: return "PUMP";
        case HydroEquipment::GrowChannel: return "GROW CHANNEL";
        case HydroEquipment::GrowLight: return "GROW LIGHT";
        case HydroEquipment::None: return "NONE";
    }
    return "NONE";
}

void DrawToast(const std::string& toast, float toastTime) {
    if (toastTime <= 0.0F || toast.empty()) return;
    ui::Panel(ui::Rect(340, 270, 600, 58), Color{36, 49, 42, 242});
    ui::CenteredText(toast.c_str(), ui::Rect(340, 270, 600, 58), 19, palette::Cream);
}
}  // namespace

void DrawOutdoorHud(const GameSession& session, FarmInteractionKind interaction,
                    int targetPlot, const std::string& toast, float toastTime) {
    ui::Panel(ui::Rect(18, 16, 344, 112), palette::Panel);
    char line[160];
    std::snprintf(line, sizeof(line), "DAY %d  %s", session.calendar.day,
                  WeekdayName(session.calendar.day));
    ui::Text(line, 34, 29, 23, palette::Cream);
    std::snprintf(line, sizeof(line), "%s    %s", FormatTime(session.calendar.minuteOfDay).c_str(),
                  session.calendar.weather == Weather::Rainy ? "RAIN" : "SUNNY");
    ui::Text(line, 34, 58, 21,
             session.calendar.weather == Weather::Rainy ? SKYBLUE : palette::Gold);
    std::snprintf(line, sizeof(line), "$%d     REP %d", session.money, session.reputation);
    ui::Text(line, 34, 88, 20, palette::Cream);

    const float energy = static_cast<float>(session.player.energy) /
                         static_cast<float>(session.player.maxEnergy);
    std::snprintf(line, sizeof(line), "ENERGY %d/%d", session.player.energy,
                  session.player.maxEnergy);
    ui::Bar(ui::Rect(18, 138, 344, 29), energy, Color{42, 129, 45, 255}, line);

    ui::Panel(ui::Rect(18, 178, 410, 72), palette::Panel);
    ui::Text(QuestTitle(session.quests.active), 34, 188, 20, palette::Gold);
    ui::Text(QuestObjectiveText(session).c_str(), 34, 218, 16, palette::Cream);

    ui::Panel(ui::Rect(350, 646, 610, 58), palette::Panel);
    for (int i = 0; i < 4; ++i) {
        const Rectangle slot = ui::Rect(362.0F + static_cast<float>(i) * 146.0F, 655, 136, 40);
        const bool selected = static_cast<int>(session.selectedTool) == i;
        DrawRectangleRec(slot, selected ? Color{43, 66, 34, 255} : Color{20, 23, 18, 245});
        if (selected) {
            DrawRectangleLinesEx(slot, 2.0F * ui::Scale(), palette::Gold);
        }
        ui::CenteredText(ToolName(session, i), slot, i == 3 ? 15.0F : 17.0F, palette::Cream);
    }

    if (interaction != FarmInteractionKind::None) {
        ui::Panel(ui::Rect(430, 575, 420, 42), Color{28, 39, 34, 230});
        ui::CenteredText(FarmInteractionPrompt(interaction), ui::Rect(430, 575, 420, 42), 18,
                         palette::Cream);
    } else if (targetPlot >= 0) {
        ui::Text("F / CLICK - Use selected tool", 500, 610, 17, palette::Cream);
    }

    std::snprintf(line, sizeof(line), "CAN %d/%d   Z/X change seed", session.farm.wateringCan,
                  session.farm.wateringCanCapacity);
    ui::Text(line, 982, 675, 16, palette::Cream);
    DrawToast(toast, toastTime);
}

void DrawHydroHud(const GameSession& session, HydroInteractionKind interaction,
                  HydroSocket targetSocket, const HydroValidationResult& validation,
                  bool validationAttempted, const std::string& toast, float toastTime) {
    char line[192];
    ui::Panel(ui::Rect(18, 16, 430, 118), palette::Panel);
    ui::Text("HYDROPONICS PROTOTYPE", 34, 29, 23, palette::Gold);
    ui::Text(QuestObjectiveText(session).c_str(), 34, 61, 17, palette::Cream);
    std::snprintf(line, sizeof(line), "COST $%d / $%d     POWER %d / %d W",
                  validation.totalCost, kHydroBuildGrant, validation.activePower,
                  kHydroAvailablePower);
    ui::Text(line, 34, 95, 18,
             validation.withinBudget && validation.withinPower ? palette::Cream
                                                                : Color{242, 112, 92, 255});

    ui::Panel(ui::Rect(337, 646, 606, 58), palette::Panel);
    const HydroEquipment equipment[] = {HydroEquipment::Reservoir, HydroEquipment::Pump,
                                        HydroEquipment::GrowChannel, HydroEquipment::GrowLight};
    for (int i = 0; i < 4; ++i) {
        const Rectangle slot = ui::Rect(350.0F + static_cast<float>(i) * 146.0F, 655, 136, 40);
        const bool selected = session.hydro.selectedEquipment == equipment[i];
        DrawRectangleRounded(slot, 0.18F, 6,
                             selected ? Color{77, 126, 111, 255} : Color{48, 61, 58, 235});
        if (selected) {
            DrawRectangleRoundedLinesEx(slot, 0.18F, 6, 2.0F * ui::Scale(), palette::Gold);
        }
        char label[48];
        std::snprintf(label, sizeof(label), "%d %s", i + 1, EquipmentName(equipment[i]));
        ui::CenteredText(label, slot, 14, palette::Cream);
    }

    if (interaction != HydroInteractionKind::None) {
        ui::Panel(ui::Rect(420, 575, 440, 42), Color{28, 39, 36, 235});
        ui::CenteredText(HydroInteractionPrompt(interaction), ui::Rect(420, 575, 440, 42), 18,
                         palette::Cream);
    } else if (targetSocket != HydroSocket::Count) {
        ui::Panel(ui::Rect(380, 575, 520, 42), Color{28, 39, 36, 235});
        ui::CenteredText("F / CLICK place     R remove", ui::Rect(380, 575, 520, 42), 18,
                         palette::Cream);
    }

    if (validationAttempted && !validation.IsValid()) {
        ui::Panel(ui::Rect(18, 150, 430, 32.0F + static_cast<float>(validation.issues.size()) * 24.0F),
                  Color{61, 40, 37, 238});
        ui::Text("CONSOLE REPORT", 34, 161, 18, Color{242, 151, 105, 255});
        float y = 189.0F;
        for (const auto& issue : validation.issues) {
            ui::Text(issue.c_str(), 34, y, 15, palette::Cream);
            y += 24.0F;
        }
    } else if (validationAttempted && validation.IsValid()) {
        ui::Panel(ui::Rect(18, 150, 430, 62), Color{38, 78, 61, 238});
        ui::Text("CONSOLE: FLOW, LIGHT, POWER, AND BUDGET VALID", 34, 171, 15,
                 Color{139, 232, 166, 255});
    }

    DrawToast(toast, toastTime);
}

}  // namespace ultradope
