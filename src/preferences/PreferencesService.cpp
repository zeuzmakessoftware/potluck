#include "preferences/PreferencesService.hpp"

#include "io/AppPaths.hpp"
#include "preferences/PreferencesCodec.hpp"

#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>
#include <utility>

#if defined(_WIN32) && __has_include(<windows.h>)
#define WIN32_LEAN_AND_MEAN
#define POTLUCK_WINDOWS_REPLACE 1
#include <windows.h>
#endif

namespace ultradope {
namespace {
std::string TemporarySuffix() {
    static std::atomic<unsigned long long> sequence{0};
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::to_string(timestamp) + "." + std::to_string(sequence.fetch_add(1));
}

bool ReplaceFile(const std::filesystem::path& source,
                 const std::filesystem::path& destination,
                 std::error_code& error) {
#if defined(POTLUCK_WINDOWS_REPLACE)
    if (MoveFileExW(source.c_str(), destination.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0) {
        error.clear();
        return true;
    }
    error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
    return false;
#else
    std::filesystem::rename(source, destination, error);
    return !error;
#endif
}

bool ReadAll(const std::filesystem::path& path, std::string& contents,
             std::string& message) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        message = "Could not open preferences.";
        return false;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (!input.good() && !input.eof()) {
        message = "Could not read preferences.";
        return false;
    }

    contents = buffer.str();
    return true;
}
}  // namespace

PreferencesService::PreferencesService()
    : PreferencesService(PotluckApplicationDataRoot() / "preferences.cfg") {}

PreferencesService::PreferencesService(std::filesystem::path preferencesPath)
    : preferencesPath_(std::move(preferencesPath)) {}

bool PreferencesService::Load(AppPreferences& preferences, std::string& message) const {
    std::error_code filesystemError;
    const bool exists = std::filesystem::exists(preferencesPath_, filesystemError);
    if (filesystemError) {
        message = "Could not inspect the preferences file.";
        return false;
    }
    if (!exists) {
        message.clear();
        return true;
    }

    std::string encoded;
    if (!ReadAll(preferencesPath_, encoded, message)) return false;

    std::string error;
    AppPreferences loaded;
    if (!PreferencesCodec::Decode(encoded, loaded, error)) {
        const auto rejectedPath = preferencesPath_.string() + ".rejected";
        std::filesystem::copy_file(preferencesPath_, rejectedPath,
                                   std::filesystem::copy_options::overwrite_existing,
                                   filesystemError);
        message = error;
        if (filesystemError) {
            message += " The rejected file could not be backed up.";
        } else {
            message += " The rejected file was preserved beside the preferences file.";
        }
        return false;
    }

    preferences = loaded;
    message.clear();
    return true;
}

bool PreferencesService::Save(const AppPreferences& preferences, std::string& message) const {
    if (!preferences.IsValid()) {
        message = "Preferences contain invalid values.";
        return false;
    }

    std::error_code error;
    const auto parent = preferencesPath_.parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent, error);
    if (error) {
        message = "Could not create the preferences directory.";
        return false;
    }

    const auto temporary = preferencesPath_.string() + ".tmp." + TemporarySuffix();
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            message = "Could not open preferences for writing.";
            return false;
        }
        output << PreferencesCodec::Encode(preferences);
        output.flush();
        if (!output.good()) {
            output.close();
            std::filesystem::remove(temporary, error);
            message = "Could not write preferences.";
            return false;
        }
        output.close();
        if (output.fail()) {
            std::filesystem::remove(temporary, error);
            message = "Could not close preferences after writing.";
            return false;
        }
    }

    if (!ReplaceFile(temporary, preferencesPath_, error)) {
        std::error_code cleanupError;
        std::filesystem::remove(temporary, cleanupError);
        message = "Could not finalize preferences.";
        return false;
    }

    message.clear();
    return true;
}

}  // namespace ultradope
