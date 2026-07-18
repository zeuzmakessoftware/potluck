#include "TestHarness.hpp"
#include "systems/DaySystem.hpp"
#include "systems/FarmSystem.hpp"

using namespace ultradope;

void RunFarmTests() {
    GameSession session = GameSession::NewGame();
    Expect(ApplyFarmAction(session, 0, FarmAction::Till).success, "till succeeds");
    Expect(ApplyFarmAction(session, 0, FarmAction::Plant, CropType::Cinderleaf).success,
           "plant succeeds");
    Expect(ApplyFarmAction(session, 0, FarmAction::Water).success, "water succeeds");
    ExpectEqual(session.player.energy, 92, "farm actions cost energy");

    EndDay(session);
    ExpectEqual(session.farm.plots[0].growthDays, 1, "watered crop grows");
    Expect(!session.farm.plots[0].wateredToday, "water flag resets");

    session.calendar.weather = Weather::Rainy;
    EndDay(session);
    ExpectEqual(session.farm.plots[0].growthDays, 2, "rain grows crop");

    session.calendar.weather = Weather::Rainy;
    EndDay(session);
    Expect(IsMature(session.farm.plots[0]), "crop reaches maturity");
    Expect(ApplyFarmAction(session, 0, FarmAction::Harvest).success, "harvest succeeds");
    ExpectEqual(session.inventory.Count(ItemType::CinderleafFlower), 1, "harvest enters inventory");

    GameSession neglected = GameSession::NewGame();
    ApplyFarmAction(neglected, 0, FarmAction::Till);
    ApplyFarmAction(neglected, 0, FarmAction::Plant, CropType::Cinderleaf);
    neglected.calendar.weather = Weather::Sunny;
    EndDay(neglected);
    Expect(neglected.farm.plots[0].withered, "untended crop withers");
}
