#pragma once

#include "domain/GameSession.hpp"

#include <cstddef>
#include <string>

namespace ultradope {

enum class FarmAction { Till, Plant, Water, Harvest };

struct ActionResult {
    bool success = false;
    std::string message;
};

ActionResult ApplyFarmAction(GameSession& session, std::size_t plotIndex,
                             FarmAction action, CropType crop = CropType::None);
ActionResult RefillWateringCan(GameSession& session);

}  // namespace ultradope
