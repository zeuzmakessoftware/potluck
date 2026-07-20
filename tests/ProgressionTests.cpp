#include "TestHarness.hpp"
#include "systems/DaySystem.hpp"
#include "systems/EconomySystem.hpp"
#include "systems/HydroponicsSystem.hpp"
#include "systems/QuestSystem.hpp"

using namespace ultradope;

namespace {
void BuildLeftHydroSolution(HydroState& hydro) {
    PlaceHydroEquipment(hydro, HydroSocket::ReservoirBase, HydroEquipment::Reservoir);
    PlaceHydroEquipment(hydro, HydroSocket::LeftPump, HydroEquipment::Pump);
    PlaceHydroEquipment(hydro, HydroSocket::LeftChannel, HydroEquipment::GrowChannel);
    PlaceHydroEquipment(hydro, HydroSocket::CenterChannel, HydroEquipment::GrowChannel);
    PlaceHydroEquipment(hydro, HydroSocket::LeftLight, HydroEquipment::GrowLight);
    PlaceHydroEquipment(hydro, HydroSocket::CenterLight, HydroEquipment::GrowLight);
}
}  // namespace

void RunProgressionTests() {
    GameSession earlyBuild = GameSession::NewGame();
    BuildLeftHydroSolution(earlyBuild.hydro);
    Expect(!CompleteHydroBuild(earlyBuild), "hydro cannot complete before it is unlocked");
    Expect(!earlyBuild.hydro.completed, "early completion does not consume the build");

    GameSession session = GameSession::NewGame();
    RecordQuestEvent(session, QuestEvent::SpokeToTessa);
    Expect(session.quests.active == QuestId::OutdoorBasics, "early Tessa interaction is ignored");
    Expect(!session.progression.hydroUnlocked, "early Tessa does not unlock hydro");

    RecordQuestEvent(session, QuestEvent::PlotTilled, 3);
    RecordQuestEvent(session, QuestEvent::CropPlanted, 3);
    const std::string completion = RecordQuestEvent(session, QuestEvent::PlotWatered, 3);
    Expect(session.quests.active == QuestId::FirstShipment, "outdoor quest advances");
    Expect(!completion.empty(), "quest completion reports reward");
    ExpectEqual(session.money, 350, "quest reward money");

    session.inventory.Add(ItemType::CinderleafFlower, 2);
    RecordQuestEvent(session, QuestEvent::CropHarvested, 2);
    Expect(ShipOne(session, CropType::Cinderleaf).success, "ship first crop");
    Expect(ShipOne(session, CropType::Cinderleaf).success, "ship second crop");
    const int before = session.money;
    const DaySummary summary = SettleShipment(session);
    ExpectEqual(summary.revenue, 116, "shipment revenue");
    ExpectEqual(session.money, before + 116, "sale credited");
    Expect(session.quests.active == QuestId::HydroInvitation, "shipment unlocks hydro invitation");
    Expect(session.violetUnlocked, "first shipment unlocks violet comet");

    const std::string invitation = RecordQuestEvent(session, QuestEvent::SpokeToTessa);
    Expect(!invitation.empty(), "hydro invitation reports transition");
    Expect(session.quests.active == QuestId::HydroAssembly, "Tessa opens hydro assembly");
    Expect(session.progression.hydroUnlocked, "hydro workshop unlocked");
    Expect(!session.progression.hydroBriefingSeen, "hydro briefing awaits acknowledgement");

    BuildLeftHydroSolution(session.hydro);
    Expect(CompleteHydroBuild(session), "valid hydro build completes once");
    Expect(!CompleteHydroBuild(session), "hydro completion is idempotent");
    Expect(session.quests.active == QuestId::Complete, "hydro validation completes quest chain");
    Expect(session.progression.chapter == ChapterId::VerticalSliceComplete,
           "progression reaches completion chapter");

    session.progression.environment = EnvironmentId::HydroLab;
    const float labTime = session.calendar.minuteOfDay;
    Expect(!AdvanceClock(session, 60.0F), "outdoor clock pauses in hydro lab");
    Expect(session.calendar.minuteOfDay == labTime, "lab does not advance farm time");
    Expect(WeatherForDay(123U, 4) == WeatherForDay(123U, 4), "weather deterministic");

    GameSession morning = GameSession::NewGame();
    morning.progression.lastMorningDoorCutsceneDay = morning.calendar.day;
    const int viewedDay = morning.calendar.day;
    EndDay(morning);
    ExpectEqual(morning.calendar.day, viewedDay + 1, "day rollover advances calendar");
    ExpectEqual(morning.progression.lastMorningDoorCutsceneDay, viewedDay,
                "day rollover makes next morning cutscene due");
}
