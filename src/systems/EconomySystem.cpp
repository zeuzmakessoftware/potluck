#include "systems/EconomySystem.hpp"

#include "systems/QuestSystem.hpp"

namespace ultradope {
namespace {
int CropIndex(CropType crop) {
    switch (crop) {
        case CropType::Cinderleaf: return 0;
        case CropType::VioletComet: return 1;
        case CropType::Sunspun: return 2;
        case CropType::None: return -1;
    }
    return -1;
}

bool IsUnlocked(const GameSession& session, CropType crop) {
    return crop == CropType::Cinderleaf ||
           (crop == CropType::VioletComet && session.violetUnlocked) ||
           (crop == CropType::Sunspun && session.sunspunUnlocked);
}
}  // namespace

EconomyResult BuySeed(GameSession& session, CropType crop) {
    if (!IsUnlocked(session, crop)) return {false, "That cultivar has not been unlocked yet."};
    const auto& definition = GetCropDefinition(crop);
    if (session.money < definition.seedPrice) return {false, "Not enough credits."};
    if (!session.inventory.CanAdd(SeedItem(crop), 1)) return {false, "Inventory is full."};
    session.money -= definition.seedPrice;
    session.inventory.Add(SeedItem(crop), 1);
    return {true, "Seed packet purchased."};
}

EconomyResult ShipOne(GameSession& session, CropType crop) {
    const int index = CropIndex(crop);
    if (index < 0) return {false, "Select a harvested crop."};
    if (!session.inventory.Remove(HarvestItem(crop), 1)) return {false, "No harvested crop to ship."};
    ++session.shipment.cropCounts[static_cast<std::size_t>(index)];
    const std::string quest = RecordQuestEvent(session, QuestEvent::ItemShipped);
    return {true, quest.empty() ? "Added to tonight's shipment." : quest};
}

int PendingShipmentCount(const GameSession& session) {
    int total = 0;
    for (int count : session.shipment.cropCounts) total += count;
    return total;
}

DaySummary SettleShipment(GameSession& session) {
    DaySummary summary;
    summary.dayEnded = session.calendar.day;
    const CropType crops[] = {CropType::Cinderleaf, CropType::VioletComet, CropType::Sunspun};
    for (std::size_t index = 0; index < session.shipment.cropCounts.size(); ++index) {
        const int count = session.shipment.cropCounts[index];
        summary.itemsSold += count;
        summary.revenue += count * GetCropDefinition(crops[index]).salePrice;
        session.shipment.cropCounts[index] = 0;
    }
    session.money += summary.revenue;
    session.lifetimeRevenue += summary.revenue;
    if (summary.itemsSold > 0) {
        summary.note = RecordQuestEvent(session, QuestEvent::SaleCompleted);
        const std::string progression = EvaluateQuestProgress(session);
        if (!progression.empty()) summary.note = progression;
    } else {
        summary.note = "No items shipped today.";
    }
    return summary;
}

}  // namespace ultradope
