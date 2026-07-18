#pragma once

#include "preferences/AppPreferences.hpp"

#include <filesystem>
#include <string>

namespace ultradope {

class PreferencesService {
public:
    PreferencesService();
    explicit PreferencesService(std::filesystem::path preferencesPath);

    bool Load(AppPreferences& preferences, std::string& message) const;
    bool Save(const AppPreferences& preferences, std::string& message) const;

private:
    std::filesystem::path preferencesPath_;
};

}  // namespace ultradope
