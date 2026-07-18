#include "app/GameApp.hpp"

#include "render/HydroRenderer.hpp"
#include "render/WorldRenderer.hpp"
#include "systems/DaySystem.hpp"
#include "systems/EconomySystem.hpp"
#include "systems/FarmSystem.hpp"
#include "systems/HydroponicsSystem.hpp"
#include "systems/QuestSystem.hpp"
#include "ui/Hud.hpp"
#include "ui/IntroScreen.hpp"
#include "ui/ModalScreens.hpp"
#include "ui/TitleScreen.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace ultradope {
namespace {
const char* JuniperText(const GameSession& session) {
    if (session.quests.active == QuestId::OutdoorBasics) {
        return "Good growing starts with care: till, plant, then water three plots.";
    }
    if (session.quests.active == QuestId::FirstShipment) {
        return "Ship two harvested flowers, then sleep to settle the order.";
    }
    if (session.quests.active == QuestId::Complete) {
        return "The hydroponics prototype is running. Outdoor roots, indoor reach.";
    }
    return "Milo can help with seed stock. The shipping bin settles every night.";
}

const char* MiloText(const GameSession& session) {
    if (!session.violetUnlocked) {
        return "Cinderleaf is quick and forgiving. Finish a shipment and I will share more stock.";
    }
    return "Violet Comet is ready for you now. It takes longer, but rewards patience.";
}

const char* TessaText(const GameSession& session) {
    if (session.quests.active == QuestId::HydroInvitation) {
        return "I cleared the old workshop. Let us build a compact hydroponics prototype.";
    }
    if (session.quests.active == QuestId::HydroAssembly) {
        return "Fit the equipment into its sockets, then validate the build at the console.";
    }
    if (session.quests.active == QuestId::Complete) {
        return "The prototype is live. This little farm just learned a whole new way to grow.";
    }
    return "Healthy plants are patient plants. Water them, then let the day do its work.";
}

HydroEquipment EquipmentForSelection(int selection) {
    switch (selection) {
        case 0: return HydroEquipment::Reservoir;
        case 1: return HydroEquipment::Pump;
        case 2: return HydroEquipment::GrowChannel;
        case 3: return HydroEquipment::GrowLight;
        default: return HydroEquipment::None;
    }
}
}  // namespace

int GameApp::Run() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Potluck - Grow a Legacy");
    SetWindowMinSize(960, 540);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    EnableCursor();

    while (running_ && !WindowShouldClose()) {
        const float deltaTime = std::min(GetFrameTime(), 0.05F);
        Update(deltaTime);
        BeginDrawing();
        Draw();
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}

void GameApp::StartNewGame() {
    session_ = GameSession::NewGame();
    camera_.Reset(CameraProfile::Outdoor);
    introCard_ = 0;
    hydroValidation_ = ValidateHydroBuild(session_.hydro);
    hydroValidationAttempted_ = false;
    ResetTransientTargets();
    SetScreen(AppScreen::OriginIntro);
}

void GameApp::ContinueGame() {
    std::string message;
    if (saves_.Load(session_, message)) {
        ConfigureLoadedEnvironment();
        SetScreen(session_.progression.originCompleted ? AppScreen::Playing : AppScreen::OriginIntro);
    }
    ShowMessage(std::move(message));
}

void GameApp::CompleteOrigin() {
    session_.progression.originCompleted = true;
    session_.progression.era = Era::NineteenSeventies;
    session_.progression.chapter = ChapterId::OutdoorFoundations;
    session_.progression.environment = EnvironmentId::OutdoorFarm;
    session_.player.position = FarmEntrySpawn();
    session_.player.facingRadians = 3.14159F;
    camera_.Reset(CameraProfile::Outdoor);
    ResetTransientTargets();
    ShowMessage("1972. Bramble Acre is yours to learn. Find Juniper by the farmhouse.");
    SetScreen(AppScreen::Playing);
}

void GameApp::Update(float deltaTime) {
    screenInputBlocked_ = false;
    animationTime_ += deltaTime;
    toastTime_ = std::max(0.0F, toastTime_ - deltaTime);
    if (screen_ == AppScreen::Playing) UpdateGameplay(deltaTime);
}

void GameApp::UpdateGameplay(float deltaTime) {
    if (IsKeyPressed(KEY_TAB)) {
        mouseCaptured_ = !mouseCaptured_;
        if (mouseCaptured_) DisableCursor(); else EnableCursor();
    }

    const GameInput input = PollGameInput(mouseCaptured_);
    if (input.pause) {
        SetScreen(AppScreen::Pause);
        return;
    }
    if (input.inventory) {
        SetScreen(AppScreen::Inventory);
        return;
    }
    if (input.journal) {
        SetScreen(AppScreen::Journal);
        return;
    }
    if (input.save) {
        std::string message;
        saves_.Save(session_, message);
        ShowMessage(std::move(message));
    }
    if (input.load) {
        std::string message;
        if (saves_.Load(session_, message)) ConfigureLoadedEnvironment();
        ShowMessage(std::move(message));
    }

    const Vec2 movement = camera_.MovementVector(input.moveRight, input.moveForward);
    moving_ = std::fabs(movement.x) > 0.01F || std::fabs(movement.y) > 0.01F;
    if (moving_) {
        const float speed = input.sprint ? 6.2F : 4.2F;
        const Vec2 delta{movement.x * speed * deltaTime, movement.y * speed * deltaTime};
        if (session_.progression.environment == EnvironmentId::HydroLab) {
            MovePlayerInHydro(session_.player, delta);
        } else {
            MovePlayerOnFarm(session_.player, delta);
        }
        session_.player.facingRadians = std::atan2(movement.x, movement.y);
    }
    camera_.Update(session_.player, input.cameraRotationRate, input.cameraRotationDelta,
                   input.zoom, deltaTime);

    if (session_.progression.environment == EnvironmentId::HydroLab) {
        UpdateHydroGameplay(input, deltaTime);
    } else {
        UpdateOutdoorGameplay(input, deltaTime);
    }
}

void GameApp::UpdateOutdoorGameplay(const GameInput& input, float deltaTime) {
    if (input.toolSelection >= 0) {
        session_.selectedTool = static_cast<Tool>(input.toolSelection);
    }
    if (input.seedCycle != 0) {
        int crop = static_cast<int>(session_.selectedCrop) + input.seedCycle;
        if (crop < static_cast<int>(CropType::Cinderleaf)) {
            crop = static_cast<int>(CropType::Sunspun);
        }
        if (crop > static_cast<int>(CropType::Sunspun)) {
            crop = static_cast<int>(CropType::Cinderleaf);
        }
        session_.selectedCrop = static_cast<CropType>(crop);
    }

    targetPlot_ = FindTargetPlot(session_);
    farmInteraction_ = FindNearbyFarmInteraction(session_);
    hydroInteraction_ = HydroInteractionKind::None;
    targetHydroSocket_ = HydroSocket::Count;

    if (input.useTool) {
        FarmAction action = FarmAction::Till;
        if (session_.selectedTool == Tool::WateringCan) action = FarmAction::Water;
        if (session_.selectedTool == Tool::Hand) action = FarmAction::Harvest;
        if (session_.selectedTool == Tool::Seeds) action = FarmAction::Plant;
        const ActionResult result = ApplyFarmAction(
            session_, targetPlot_ < 0 ? session_.farm.plots.size()
                                     : static_cast<std::size_t>(targetPlot_),
            action, session_.selectedCrop);
        ShowMessage(result.message);
    }
    if (input.refill && farmInteraction_ == FarmInteractionKind::WaterTank) {
        ShowMessage(RefillWateringCan(session_).message);
    }
    if (input.interact) OpenFarmInteraction(farmInteraction_);
    if (AdvanceClock(session_, deltaTime)) EndCurrentDay();
}

void GameApp::UpdateHydroGameplay(const GameInput& input, float) {
    if (input.toolSelection >= 0) {
        session_.hydro.selectedEquipment = EquipmentForSelection(input.toolSelection);
    }

    targetPlot_ = -1;
    farmInteraction_ = FarmInteractionKind::None;
    targetHydroSocket_ = FindTargetHydroSocket(session_);
    hydroInteraction_ = FindNearbyHydroInteraction(session_.player);

    if (input.useTool) {
        const HydroOperationResult result = PlaceHydroEquipment(
            session_.hydro, targetHydroSocket_, session_.hydro.selectedEquipment);
        ShowMessage(result.message);
        hydroValidation_ = ValidateHydroBuild(session_.hydro);
    }
    if (input.refill) {
        const HydroOperationResult result = RemoveHydroEquipment(session_.hydro, targetHydroSocket_);
        ShowMessage(result.message);
        hydroValidation_ = ValidateHydroBuild(session_.hydro);
    }
    if (input.interact) {
        if (hydroInteraction_ == HydroInteractionKind::ExitDoor) {
            EnterEnvironment(EnvironmentId::OutdoorFarm);
        } else if (hydroInteraction_ == HydroInteractionKind::Console) {
            hydroValidation_ = ValidateHydroBuild(session_.hydro);
            hydroValidationAttempted_ = true;
            if (hydroValidation_.IsValid()) {
                if (CompleteHydroBuild(session_)) {
                    ShowMessage("Prototype validated. Flow, coverage, power, and budget are stable.");
                    SetScreen(AppScreen::VerticalSliceComplete);
                } else {
                    ShowMessage("The prototype is already online.");
                }
            } else if (!hydroValidation_.issues.empty()) {
                ShowMessage(hydroValidation_.issues.front());
            }
        }
    }
}

void GameApp::OpenFarmInteraction(FarmInteractionKind interaction) {
    switch (interaction) {
        case FarmInteractionKind::House: EndCurrentDay(); break;
        case FarmInteractionKind::Shop: SetScreen(AppScreen::Shop); break;
        case FarmInteractionKind::ShippingBin: SetScreen(AppScreen::Shipping); break;
        case FarmInteractionKind::WaterTank:
            ShowMessage(RefillWateringCan(session_).message);
            break;
        case FarmInteractionKind::Juniper:
            dialogueSpeaker_ = "JUNIPER VALE";
            dialogueText_ = JuniperText(session_);
            SetScreen(AppScreen::Dialogue);
            break;
        case FarmInteractionKind::Milo:
            dialogueSpeaker_ = "MILO ROOK";
            dialogueText_ = MiloText(session_);
            SetScreen(AppScreen::Dialogue);
            break;
        case FarmInteractionKind::Tessa: {
            dialogueSpeaker_ = "TESSA BLOOM";
            dialogueText_ = TessaText(session_);
            const std::string result = RecordQuestEvent(session_, QuestEvent::SpokeToTessa);
            if (!result.empty()) ShowMessage(result);
            SetScreen(AppScreen::Dialogue);
            break;
        }
        case FarmInteractionKind::HydroTransport:
            EnterEnvironment(EnvironmentId::HydroLab);
            break;
        case FarmInteractionKind::None:
            ShowMessage("Nothing close enough to interact with.");
            break;
    }
}

void GameApp::EnterEnvironment(EnvironmentId environment) {
    if (environment == EnvironmentId::HydroLab && !session_.progression.hydroUnlocked) {
        ShowMessage("The workshop is still locked.");
        return;
    }
    session_.progression.environment = environment;
    if (environment == EnvironmentId::HydroLab) {
        session_.player.position = HydroEntrySpawn();
        session_.player.facingRadians = 3.14159F;
        camera_.Reset(CameraProfile::Indoor);
    } else {
        session_.player.position = FarmHydroReturnSpawn();
        session_.player.facingRadians = 1.5708F;
        camera_.Reset(CameraProfile::Outdoor);
    }
    ResetTransientTargets();
    SetScreen(AppScreen::Playing);
}

void GameApp::ConfigureLoadedEnvironment() {
    camera_.Reset(session_.progression.environment == EnvironmentId::HydroLab
                      ? CameraProfile::Indoor
                      : CameraProfile::Outdoor);
    hydroValidation_ = ValidateHydroBuild(session_.hydro);
    hydroValidationAttempted_ = false;
    ResetTransientTargets();
}

void GameApp::ResetTransientTargets() {
    targetPlot_ = -1;
    targetHydroSocket_ = HydroSocket::Count;
    farmInteraction_ = FarmInteractionKind::None;
    hydroInteraction_ = HydroInteractionKind::None;
    moving_ = false;
}

void GameApp::EndCurrentDay() {
    EndDay(session_);
    std::string message;
    saves_.Save(session_, message);
    SetScreen(AppScreen::DaySummary);
}

void GameApp::SetScreen(AppScreen screen) {
    screen_ = screen;
    screenInputBlocked_ = true;
    const bool gameplay = screen == AppScreen::Playing;
    mouseCaptured_ = gameplay;
    if (gameplay) DisableCursor(); else EnableCursor();
}

void GameApp::ShowMessage(std::string message) {
    if (message.empty()) return;
    toast_ = std::move(message);
    toastTime_ = 4.0F;
}

void GameApp::DrawGameWorld() const {
    if (session_.progression.environment == EnvironmentId::HydroLab) {
        ClearBackground(Color{40, 50, 48, 255});
        BeginMode3D(camera_.Camera());
        DrawHydroWorld(session_, targetHydroSocket_, hydroValidationAttempted_,
                       hydroValidation_, animationTime_, moving_);
        EndMode3D();
        DrawHydroHud(session_, hydroInteraction_, targetHydroSocket_, hydroValidation_,
                     hydroValidationAttempted_, toast_, toastTime_);
    } else {
        ClearBackground(SkyColorForTime(session_));
        BeginMode3D(camera_.Camera());
        DrawOutdoorWorld(session_, targetPlot_, animationTime_, moving_);
        EndMode3D();
        DrawOutdoorHud(session_, farmInteraction_, targetPlot_, toast_, toastTime_);
    }
}

void GameApp::Draw() {
    if (screen_ == AppScreen::Title) {
        const TitleAction action = DrawTitleScreen(saves_.HasSave());
        if (action == TitleAction::NewGame) StartNewGame();
        if (action == TitleAction::Continue) ContinueGame();
        if (action == TitleAction::Quit) running_ = false;
        return;
    }
    if (screen_ == AppScreen::OriginIntro) {
        const IntroAction action = DrawIntroScreen(introCard_);
        if (action == IntroAction::Advance) ++introCard_;
        if (action == IntroAction::Complete || action == IntroAction::Skip) CompleteOrigin();
        return;
    }

    DrawGameWorld();
    switch (screen_) {
        case AppScreen::Playing: break;
        case AppScreen::Pause: {
            const PauseAction action = DrawPauseScreen();
            if (screenInputBlocked_) break;
            if (action == PauseAction::Resume) SetScreen(AppScreen::Playing);
            if (action == PauseAction::Save) {
                std::string message;
                saves_.Save(session_, message);
                ShowMessage(std::move(message));
            }
            if (action == PauseAction::Load) {
                std::string message;
                if (saves_.Load(session_, message)) ConfigureLoadedEnvironment();
                ShowMessage(std::move(message));
            }
            if (action == PauseAction::Title) SetScreen(AppScreen::Title);
            break;
        }
        case AppScreen::Shop: {
            if (IsKeyPressed(KEY_ESCAPE)) SetScreen(AppScreen::Playing);
            const CropType purchase = DrawShopScreen(session_);
            if (purchase != CropType::None) ShowMessage(BuySeed(session_, purchase).message);
            break;
        }
        case AppScreen::Shipping: {
            if (IsKeyPressed(KEY_ESCAPE)) SetScreen(AppScreen::Playing);
            const CropType crop = DrawShippingScreen(session_);
            if (crop != CropType::None) ShowMessage(ShipOne(session_, crop).message);
            break;
        }
        case AppScreen::Inventory: {
            const bool close = DrawInventoryScreen(session_);
            if (!screenInputBlocked_ && close) SetScreen(AppScreen::Playing);
            break;
        }
        case AppScreen::Journal: {
            const bool close = DrawJournalScreen(session_);
            if (!screenInputBlocked_ && close) SetScreen(AppScreen::Playing);
            break;
        }
        case AppScreen::Dialogue:
            if (DrawDialogueScreen(session_, dialogueSpeaker_.c_str(), dialogueText_.c_str()) &&
                !screenInputBlocked_) {
                if (session_.progression.hydroUnlocked &&
                    !session_.progression.hydroBriefingSeen) {
                    SetScreen(AppScreen::HydroUnlocked);
                } else {
                    SetScreen(AppScreen::Playing);
                }
            }
            break;
        case AppScreen::DaySummary:
            if (DrawDaySummaryScreen(session_.lastSummary)) SetScreen(AppScreen::Playing);
            break;
        case AppScreen::HydroUnlocked: {
            const HydroUnlockAction action = DrawHydroUnlockedScreen();
            if (!screenInputBlocked_ && action != HydroUnlockAction::None) {
                session_.progression.hydroBriefingSeen = true;
                if (action == HydroUnlockAction::EnterWorkshop) {
                    EnterEnvironment(EnvironmentId::HydroLab);
                } else {
                    SetScreen(AppScreen::Playing);
                }
            }
            break;
        }
        case AppScreen::VerticalSliceComplete: {
            const bool acknowledged = DrawVerticalSliceCompleteScreen();
            if (!screenInputBlocked_ && acknowledged) {
                session_.hydro.completionAcknowledged = true;
                SetScreen(AppScreen::Playing);
            }
            break;
        }
        case AppScreen::Title:
        case AppScreen::OriginIntro:
            break;
    }
}

}  // namespace ultradope
