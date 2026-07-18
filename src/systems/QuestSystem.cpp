#include "systems/QuestSystem.hpp"

#include <algorithm>
#include <sstream>

namespace ultradope {

const char* QuestTitle(QuestId quest) {
    switch (quest) {
        case QuestId::OutdoorBasics: return "Outdoor Basics";
        case QuestId::FirstShipment: return "First Shipment";
        case QuestId::HydroInvitation: return "Hydro Invitation";
        case QuestId::HydroAssembly: return "Hydro Assembly";
        case QuestId::Complete: return "Vertical Slice Complete";
    }
    return "Unknown Quest";
}

std::string EvaluateQuestProgress(GameSession& session) {
    auto& quest = session.quests;
    if (quest.active == QuestId::OutdoorBasics && quest.tilled >= 3 &&
        quest.planted >= 3 && quest.watered >= 3) {
        quest.active = QuestId::FirstShipment;
        session.money += 100;
        session.reputation += 5;
        return "Outdoor basics complete! +100 credits, +5 reputation";
    }
    if (quest.active == QuestId::FirstShipment && quest.harvested >= 2 &&
        quest.shipped >= 2 && quest.salesCompleted >= 1) {
        quest.active = QuestId::HydroInvitation;
        session.violetUnlocked = true;
        return "First shipment complete! Violet Comet unlocked. Speak with Tessa.";
    }
    if (quest.active == QuestId::HydroInvitation && quest.spokeToTessa) {
        quest.active = QuestId::HydroAssembly;
        session.progression.hydroUnlocked = true;
        session.progression.hydroBriefingSeen = false;
        session.progression.chapter = ChapterId::HydroponicsPrototype;
        session.progression.era = Era::NineteenSeventies;
        return "Tessa opened the hydroponics workshop.";
    }
    if (quest.active == QuestId::HydroAssembly && quest.hydroBuildValidated) {
        quest.active = QuestId::Complete;
        session.progression.chapter = ChapterId::VerticalSliceComplete;
        return "Hydroponics prototype complete!";
    }
    return {};
}

std::string RecordQuestEvent(GameSession& session, QuestEvent event, int amount) {
    if (amount <= 0) return {};

    auto& quest = session.quests;
    switch (quest.active) {
        case QuestId::OutdoorBasics:
            switch (event) {
                case QuestEvent::PlotTilled: quest.tilled += amount; break;
                case QuestEvent::CropPlanted: quest.planted += amount; break;
                case QuestEvent::PlotWatered: quest.watered += amount; break;
                default: break;
            }
            break;
        case QuestId::FirstShipment:
            switch (event) {
                case QuestEvent::CropHarvested: quest.harvested += amount; break;
                case QuestEvent::ItemShipped: quest.shipped += amount; break;
                case QuestEvent::SaleCompleted: quest.salesCompleted += amount; break;
                default: break;
            }
            break;
        case QuestId::HydroInvitation:
            if (event == QuestEvent::SpokeToTessa && !quest.spokeToTessa) {
                quest.spokeToTessa = true;
            }
            break;
        case QuestId::HydroAssembly:
            if (event == QuestEvent::HydroBuildValidated) {
                quest.hydroBuildValidated = true;
            }
            break;
        case QuestId::Complete:
            break;
    }
    return EvaluateQuestProgress(session);
}

std::string QuestObjectiveText(const GameSession& session) {
    const auto& q = session.quests;
    std::ostringstream out;
    switch (q.active) {
        case QuestId::OutdoorBasics:
            out << "Till " << std::min(q.tilled, 3) << "/3  Plant "
                << std::min(q.planted, 3) << "/3  Water " << std::min(q.watered, 3) << "/3";
            break;
        case QuestId::FirstShipment:
            out << "Harvest " << std::min(q.harvested, 2) << "/2  Ship "
                << std::min(q.shipped, 2) << "/2  Settle " << std::min(q.salesCompleted, 1) << "/1";
            break;
        case QuestId::HydroInvitation:
            out << "Speak with Tessa about the hydroponics workshop.";
            break;
        case QuestId::HydroAssembly:
            out << "Validate a working hydroponics build.";
            break;
        case QuestId::Complete:
            out << "Inspect the hydroponics workshop or return to the farm.";
            break;
    }
    return out.str();
}

}  // namespace ultradope
