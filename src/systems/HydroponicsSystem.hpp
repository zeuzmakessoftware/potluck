#pragma once

#include "domain/GameSession.hpp"

namespace ultradope {

// Marks a previously valid build complete and records its quest event once.
// Returns true only when this call changes the session.
bool CompleteHydroBuild(GameSession& session);

}  // namespace ultradope
