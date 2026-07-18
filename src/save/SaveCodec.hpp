#pragma once

#include "domain/GameSession.hpp"

#include <string>
#include <string_view>

namespace ultradope {

class SaveCodec {
public:
    static std::string Encode(const GameSession& session);
    static bool Decode(std::string_view text, GameSession& output, std::string& error);
};

}  // namespace ultradope
