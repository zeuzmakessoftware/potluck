#pragma once

#include "domain/GameSession.hpp"
#include "preferences/AppPreferences.hpp"

namespace ultradope {

enum class PauseAction { None, Resume, Save, Load, Settings, Title };
enum class SettingsAction {
    None,
    ToggleCameraInversion,
    DecreaseCameraSensitivity,
    IncreaseCameraSensitivity,
    ResetDefaults,
    Back
};
enum class HydroUnlockAction { None, EnterWorkshop, StayOnFarm };

PauseAction DrawPauseScreen();
SettingsAction DrawSettingsScreen(const AppPreferences& preferences,
                                  const char* persistenceError);
CropType DrawShopScreen(const GameSession& session);
CropType DrawShippingScreen(const GameSession& session);
bool DrawInventoryScreen(const GameSession& session);
bool DrawJournalScreen(const GameSession& session);
bool DrawDialogueScreen(const GameSession& session, const char* speaker, const char* text);
bool DrawDaySummaryScreen(const DaySummary& summary);
HydroUnlockAction DrawHydroUnlockedScreen();
bool DrawVerticalSliceCompleteScreen();

}  // namespace ultradope
