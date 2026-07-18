#include "systems/FarmSystem.hpp"

#include "systems/QuestSystem.hpp"

namespace ultradope {
namespace {
ActionResult Fail(const char* message) { return {false, message}; }

std::string WithQuestMessage(const char* actionMessage, std::string questMessage) {
    if (questMessage.empty()) return actionMessage;
    return std::string(actionMessage) + "  " + questMessage;
}
}  // namespace

ActionResult ApplyFarmAction(GameSession& session, std::size_t plotIndex,
                             FarmAction action, CropType crop) {
    if (plotIndex >= session.farm.plots.size()) return Fail("No plot in range.");
    auto& plot = session.farm.plots[plotIndex];

    switch (action) {
        case FarmAction::Till: {
            if (plot.tilled) return Fail("That soil is already tilled.");
            if (session.player.energy < 4) return Fail("Too tired to till.");
            plot.tilled = true;
            session.player.energy -= 4;
            return {true, WithQuestMessage("Soil tilled.",
                RecordQuestEvent(session, QuestEvent::PlotTilled))};
        }
        case FarmAction::Plant: {
            if (!plot.tilled) return Fail("Till the soil first.");
            if (plot.crop != CropType::None) return Fail("That plot is occupied.");
            if (crop == CropType::None) return Fail("Select a seed first.");
            if (session.player.energy < 2) return Fail("Too tired to plant.");
            if (!session.inventory.Remove(SeedItem(crop), 1)) return Fail("No seeds in your pack.");
            plot.crop = crop;
            plot.growthDays = 0;
            plot.neglectDays = 0;
            plot.wateredToday = false;
            plot.withered = false;
            session.player.energy -= 2;
            return {true, WithQuestMessage("Seed planted.",
                RecordQuestEvent(session, QuestEvent::CropPlanted))};
        }
        case FarmAction::Water: {
            if (plot.crop == CropType::None) return Fail("Nothing is planted there.");
            if (plot.withered) return Fail("This crop has withered.");
            if (plot.wateredToday) return Fail("Already watered today.");
            if (session.farm.wateringCan <= 0) return Fail("Watering can is empty.");
            if (session.player.energy < 2) return Fail("Too tired to water.");
            plot.wateredToday = true;
            --session.farm.wateringCan;
            session.player.energy -= 2;
            return {true, WithQuestMessage("Crop watered.",
                RecordQuestEvent(session, QuestEvent::PlotWatered))};
        }
        case FarmAction::Harvest: {
            if (plot.crop == CropType::None) return Fail("Nothing to harvest.");
            if (plot.withered) {
                plot.crop = CropType::None;
                plot.withered = false;
                plot.growthDays = 0;
                return {true, "Withered crop cleared."};
            }
            if (!IsMature(plot)) return Fail("This crop is not ready.");
            if (session.player.energy < 2) return Fail("Too tired to harvest.");
            const ItemType flower = HarvestItem(plot.crop);
            if (!session.inventory.Add(flower, 1)) return Fail("Inventory is full.");
            plot.crop = CropType::None;
            plot.growthDays = 0;
            plot.neglectDays = 0;
            plot.wateredToday = false;
            session.player.energy -= 2;
            return {true, WithQuestMessage("Harvest gathered.",
                RecordQuestEvent(session, QuestEvent::CropHarvested))};
        }
    }
    return Fail("Unknown farm action.");
}

ActionResult RefillWateringCan(GameSession& session) {
    if (session.farm.wateringCan == session.farm.wateringCanCapacity) {
        return Fail("Watering can is already full.");
    }
    session.farm.wateringCan = session.farm.wateringCanCapacity;
    return {true, "Watering can refilled."};
}

}  // namespace ultradope
