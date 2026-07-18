#pragma once

#include "domain/GameSession.hpp"

#include <string>

namespace ultradope {

struct EconomyResult {
    bool success = false;
    std::string message;
};

EconomyResult BuySeed(GameSession& session, CropType crop);
EconomyResult ShipOne(GameSession& session, CropType crop);
int PendingShipmentCount(const GameSession& session);
DaySummary SettleShipment(GameSession& session);

}  // namespace ultradope
