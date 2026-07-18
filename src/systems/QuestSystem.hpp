#pragma once

#include "domain/GameSession.hpp"

#include <string>

namespace ultradope {

std::string RecordQuestEvent(GameSession& session, QuestEvent event, int amount = 1);
std::string EvaluateQuestProgress(GameSession& session);
std::string QuestObjectiveText(const GameSession& session);

}  // namespace ultradope
