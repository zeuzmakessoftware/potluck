#pragma once

#include "camera/ThirdPersonCamera.hpp"
#include "domain/GameSession.hpp"
#include "input/GameInput.hpp"
#include "preferences/AppPreferences.hpp"
#include "preferences/PreferencesService.hpp"
#include "save/SaveService.hpp"
#include "world/FarmLayout.hpp"
#include "world/HydroLayout.hpp"

#include <string>

namespace ultradope {

enum class AppScreen {
    Title,
    OriginIntro,
    Playing,
    Pause,
    Settings,
    Shop,
    Shipping,
    Inventory,
    Journal,
    Dialogue,
    DaySummary,
    HydroUnlocked,
    VerticalSliceComplete
};

class GameApp {
public:
    int Run();

private:
    void StartNewGame();
    void ContinueGame();
    void CompleteOrigin();
    void Update(float deltaTime);
    void UpdateGameplay(float deltaTime);
    void UpdateOutdoorGameplay(const GameInput& input, float deltaTime);
    void UpdateHydroGameplay(const GameInput& input, float deltaTime);
    void Draw();
    void DrawGameWorld() const;
    void OpenFarmInteraction(FarmInteractionKind interaction);
    void EnterEnvironment(EnvironmentId environment);
    void ConfigureLoadedEnvironment();
    void ResetTransientTargets();
    void EndCurrentDay();
    void LoadPreferences();
    void PersistPreferences();
    void SetScreen(AppScreen screen);
    void ShowMessage(std::string message);

    GameSession session_ = GameSession::NewGame();
    SaveService saves_;
    AppPreferences preferences_;
    PreferencesService preferencesService_;
    ThirdPersonCamera camera_;
    AppScreen screen_ = AppScreen::Title;
    FarmInteractionKind farmInteraction_ = FarmInteractionKind::None;
    HydroInteractionKind hydroInteraction_ = HydroInteractionKind::None;
    HydroSocket targetHydroSocket_ = HydroSocket::Count;
    HydroValidationResult hydroValidation_;
    int targetPlot_ = -1;
    int introCard_ = 0;
    bool running_ = true;
    bool mouseCaptured_ = false;
    bool moving_ = false;
    bool hydroValidationAttempted_ = false;
    bool screenInputBlocked_ = false;
    float screenInputBlockTime_ = 0.0F;
    float animationTime_ = 0.0F;
    float toastTime_ = 0.0F;
    std::string toast_;
    std::string settingsError_;
    std::string dialogueSpeaker_;
    std::string dialogueText_;
};

}  // namespace ultradope
