#pragma once

#include <cstdlib>
#include <filesystem>

namespace ultradope {
namespace detail {
inline const char* NonEmptyEnvironment(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && value[0] != '\0' ? value : nullptr;
}
}  // namespace detail

inline std::filesystem::path PotluckApplicationDataRoot() {
#if defined(_WIN32)
    if (const char* appData = detail::NonEmptyEnvironment("APPDATA")) {
        return std::filesystem::path(appData) / "Potluck";
    }
    if (const char* profile = detail::NonEmptyEnvironment("USERPROFILE")) {
        return std::filesystem::path(profile) / "AppData" / "Roaming" / "Potluck";
    }
#elif defined(__APPLE__)
    if (const char* home = detail::NonEmptyEnvironment("HOME")) {
        return std::filesystem::path(home) / "Library" / "Application Support" / "Potluck";
    }
#else
    if (const char* config = detail::NonEmptyEnvironment("XDG_CONFIG_HOME")) {
        return std::filesystem::path(config) / "Potluck";
    }
    if (const char* home = detail::NonEmptyEnvironment("HOME")) {
        return std::filesystem::path(home) / ".config" / "Potluck";
    }
#endif
    return std::filesystem::path("Potluck");
}

}  // namespace ultradope
