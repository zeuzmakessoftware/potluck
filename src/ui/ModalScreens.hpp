#pragma once

#include "domain/GameSession.hpp"

namespace ultradope {

enum class PauseAction { None, Resume, Save, Load, Title };
enum class HydroUnlockAction { None, EnterWorkshop, StayOnFarm };

PauseAction DrawPauseScreen();
CropType DrawShopScreen(const GameSession& session);
CropType DrawShippingScreen(const GameSession& session);
bool DrawInventoryScreen(const GameSession& session);
bool DrawJournalScreen(const GameSession& session);
bool DrawDialogueScreen(const GameSession& session, const char* speaker, const char* text);
bool DrawDaySummaryScreen(const DaySummary& summary);
HydroUnlockAction DrawHydroUnlockedScreen();
bool DrawVerticalSliceCompleteScreen();

}  // namespace ultradope
