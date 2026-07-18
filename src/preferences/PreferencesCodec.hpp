#pragma once

#include "preferences/AppPreferences.hpp"

#include <string>
#include <string_view>

namespace ultradope {

class PreferencesCodec {
public:
    static std::string Encode(const AppPreferences& preferences);
    static bool Decode(std::string_view text, AppPreferences& output, std::string& error);
};

}  // namespace ultradope
