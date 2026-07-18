#include "TestHarness.hpp"
#include "save/SaveCodec.hpp"
#include "systems/FarmSystem.hpp"

#include <string>

using namespace ultradope;

namespace {
void InstallValidHydroBuild(HydroState& hydro) {
    PlaceHydroEquipment(hydro, HydroSocket::ReservoirBase, HydroEquipment::Reservoir);
    PlaceHydroEquipment(hydro, HydroSocket::LeftPump, HydroEquipment::Pump);
    PlaceHydroEquipment(hydro, HydroSocket::LeftChannel, HydroEquipment::GrowChannel);
    PlaceHydroEquipment(hydro, HydroSocket::CenterChannel, HydroEquipment::GrowChannel);
    PlaceHydroEquipment(hydro, HydroSocket::LeftLight, HydroEquipment::GrowLight);
    PlaceHydroEquipment(hydro, HydroSocket::CenterLight, HydroEquipment::GrowLight);
}

}  // namespace

void RunSaveCodecTests() {
    GameSession source = GameSession::NewGame();
    ApplyFarmAction(source, 0, FarmAction::Till);
    ApplyFarmAction(source, 0, FarmAction::Plant, CropType::Cinderleaf);
    source.money = 777;
    source.calendar.day = 6;
    source.calendar.weather = Weather::Rainy;
    source.lastSummary.note = "A quoted note survives";
    source.progression.originCompleted = true;
    source.progression.era = Era::NineteenSeventies;
    source.progression.chapter = ChapterId::VerticalSliceComplete;
    source.progression.environment = EnvironmentId::HydroLab;
    source.progression.hydroUnlocked = true;
    source.progression.hydroBriefingSeen = true;
    source.quests.active = QuestId::Complete;
    source.quests.spokeToTessa = true;
    source.quests.hydroBuildValidated = true;
    source.hydro.selectedEquipment = HydroEquipment::GrowLight;
    InstallValidHydroBuild(source.hydro);
    source.hydro.completed = true;
    source.hydro.completionAcknowledged = true;

    const std::string encoded = SaveCodec::Encode(source);
    Expect(encoded.rfind("POTLUCK 1\n", 0) == 0, "Potluck save header");

    GameSession decoded;
    std::string error;
    Expect(SaveCodec::Decode(encoded, decoded, error), "save round trip");
    ExpectEqual(decoded.money, 777, "money round trip");
    ExpectEqual(decoded.calendar.day, 6, "day round trip");
    Expect(decoded.calendar.weather == Weather::Rainy, "weather round trip");
    Expect(decoded.farm.plots[0].crop == CropType::Cinderleaf, "crop round trip");
    ExpectEqual(decoded.inventory.Count(ItemType::CinderleafSeed), 3, "inventory round trip");
    Expect(decoded.lastSummary.note == source.lastSummary.note, "quoted string round trip");
    Expect(decoded.progression.environment == EnvironmentId::HydroLab,
           "environment round trip");
    Expect(decoded.progression.chapter == ChapterId::VerticalSliceComplete,
           "chapter round trip");
    Expect(decoded.progression.hydroBriefingSeen, "briefing flag round trip");
    Expect(decoded.hydro.selectedEquipment == HydroEquipment::GrowLight,
           "hydro selection round trip");
    Expect(decoded.hydro.equipment == source.hydro.equipment, "all placements round trip");
    Expect(decoded.hydro.completed && decoded.hydro.completionAcknowledged,
           "hydro completion round trip");

    GameSession unchanged = GameSession::NewGame();
    unchanged.money = 123;
    Expect(!SaveCodec::Decode("ULTRADOPE 1\nEND\n", unchanged, error),
           "reject old Ultradope header");
    ExpectEqual(unchanged.money, 123, "old header leaves output unchanged");
    Expect(!SaveCodec::Decode("POTLUCK 99\nEND\n", unchanged, error), "reject future version");
    Expect(!SaveCodec::Decode("POTLUCK 1\nEND\n", unchanged, error), "reject truncation");

    const std::string lockedLab = ReplaceOnce(
        encoded, "PROGRESSION 1 3 1 1 1 1", "PROGRESSION 1 3 1 1 0 0");
    Expect(!SaveCodec::Decode(lockedLab, unchanged, error),
           "reject hydro lab while workshop locked");

    const std::string incompatiblePlacement = ReplaceOnce(
        encoded, "HYDRO 4 1 1 1 2", "HYDRO 4 1 1 2 2");
    Expect(!SaveCodec::Decode(incompatiblePlacement, unchanged, error),
           "reject equipment in incompatible socket");
    ExpectEqual(unchanged.money, 123, "invalid data leaves output unchanged");
}
