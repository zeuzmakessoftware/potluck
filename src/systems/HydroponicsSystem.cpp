#include "systems/HydroponicsSystem.hpp"

#include "systems/QuestSystem.hpp"

namespace ultradope {

bool CompleteHydroBuild(GameSession& session) {
    if (!session.progression.hydroUnlocked || session.quests.active != QuestId::HydroAssembly ||
        session.hydro.completed || !ValidateHydroBuild(session.hydro).IsValid()) {
        return false;
    }
    session.hydro.completed = true;
    RecordQuestEvent(session, QuestEvent::HydroBuildValidated);
    return true;
}

}  // namespace ultradope
