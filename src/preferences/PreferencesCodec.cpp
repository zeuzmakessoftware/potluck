#include "preferences/PreferencesCodec.hpp"

#include <iomanip>
#include <limits>
#include <sstream>

namespace ultradope {
namespace {
bool HasExtraValue(std::istringstream& line) {
    std::string extra;
    return static_cast<bool>(line >> extra);
}
}  // namespace

std::string PreferencesCodec::Encode(const AppPreferences& preferences) {
    std::ostringstream output;
    output << "POTLUCK_PREFERENCES 1\n";
    output << "INVERT_HORIZONTAL_CAMERA " << preferences.invertHorizontalCamera << '\n';
    output << "CAMERA_SENSITIVITY "
           << std::setprecision(std::numeric_limits<float>::max_digits10)
           << preferences.cameraSensitivity << '\n';
    output << "END\n";
    return output.str();
}

bool PreferencesCodec::Decode(std::string_view text, AppPreferences& output, std::string& error) {
    std::istringstream input{std::string(text)};
    std::string header;
    if (!std::getline(input, header)) {
        error = "Preferences file is empty.";
        return false;
    }

    std::istringstream headerLine(header);
    std::string magic;
    int version = 0;
    if (!(headerLine >> magic >> version) || HasExtraValue(headerLine) ||
        magic != "POTLUCK_PREFERENCES" || version != 1) {
        error = "Unsupported or malformed preferences header.";
        return false;
    }

    AppPreferences candidate;
    bool foundInversion = false;
    bool foundSensitivity = false;
    bool foundEnd = false;
    std::string rawLine;
    while (std::getline(input, rawLine)) {
        std::istringstream line(rawLine);
        std::string key;
        if (!(line >> key)) continue;

        if (key == "INVERT_HORIZONTAL_CAMERA") {
            int value = 0;
            if (foundInversion || !(line >> value) || HasExtraValue(line) ||
                (value != 0 && value != 1)) {
                error = "Invalid horizontal camera preference.";
                return false;
            }
            candidate.invertHorizontalCamera = value == 1;
            foundInversion = true;
        } else if (key == "CAMERA_SENSITIVITY") {
            float value = 0.0F;
            if (foundSensitivity || !(line >> value) || HasExtraValue(line)) {
                error = "Invalid camera sensitivity preference.";
                return false;
            }
            candidate.cameraSensitivity = value;
            if (!candidate.IsValid()) {
                error = "Invalid camera sensitivity preference.";
                return false;
            }
            foundSensitivity = true;
        } else if (key == "END") {
            if (HasExtraValue(line)) {
                error = "Malformed preferences terminator.";
                return false;
            }
            foundEnd = true;
            break;
        } else {
            error = "Unknown preferences field.";
            return false;
        }
    }

    if (!foundEnd) {
        error = "Preferences file is incomplete.";
        return false;
    }
    while (std::getline(input, rawLine)) {
        if (rawLine.find_first_not_of(" \t\r") != std::string::npos) {
            error = "Preferences file has trailing data.";
            return false;
        }
    }

    output = candidate;
    error.clear();
    return true;
}

}  // namespace ultradope
