#include "TestHarness.hpp"
#include "preferences/AppPreferences.hpp"
#include "preferences/PreferencesCodec.hpp"
#include "preferences/PreferencesService.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>

using namespace ultradope;

void RunPreferencesTests() {
    AppPreferences defaults;
    Expect(!defaults.invertHorizontalCamera, "camera inversion defaults off");
    Expect(defaults.cameraSensitivity == 1.0F, "camera sensitivity defaults to 100 percent");
    Expect(defaults.CameraRotationScale() == -1.0F, "new standard camera direction is default");
    Expect(defaults.IsValid(), "default preferences are valid");
    Expect(defaults.CanDecreaseCameraSensitivity(), "default sensitivity can decrease");
    Expect(defaults.CanIncreaseCameraSensitivity(), "default sensitivity can increase");
    Expect(defaults.DecreaseCameraSensitivity(), "decrease sensitivity");
    Expect(defaults.cameraSensitivity == 0.75F, "decrease uses configured step");
    Expect(defaults.IncreaseCameraSensitivity(), "increase sensitivity");
    Expect(defaults.cameraSensitivity == 1.0F, "increase restores default sensitivity");

    defaults.invertHorizontalCamera = true;
    Expect(defaults.CameraRotationScale() == 1.0F, "inversion restores legacy camera direction");
    defaults.cameraSensitivity = 1.5F;
    Expect(defaults.CameraRotationScale() == 1.5F, "inversion composes with sensitivity");
    defaults.invertHorizontalCamera = false;
    Expect(defaults.CameraRotationScale() == -1.5F, "standard direction composes with sensitivity");

    const std::string encoded = PreferencesCodec::Encode(defaults);
    AppPreferences decoded;
    std::string error;
    Expect(PreferencesCodec::Decode(encoded, decoded, error), "preference round trip");
    Expect(!decoded.invertHorizontalCamera, "inversion round trip");
    Expect(decoded.cameraSensitivity == 1.5F, "sensitivity round trip");

    AppPreferences precise;
    precise.cameraSensitivity = 1.234F;
    Expect(PreferencesCodec::Decode(PreferencesCodec::Encode(precise), decoded, error),
           "precise sensitivity round trip");
    Expect(decoded.cameraSensitivity == precise.cameraSensitivity,
           "codec preserves accepted float values");

    AppPreferences unchanged;
    unchanged.invertHorizontalCamera = true;
    unchanged.cameraSensitivity = 2.0F;
    Expect(!PreferencesCodec::Decode("POTLUCK_PREFERENCES 99\nEND\n", unchanged, error),
           "reject future preference version");
    Expect(unchanged.invertHorizontalCamera && unchanged.cameraSensitivity == 2.0F,
           "invalid preferences leave output unchanged");

    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "INVERT_HORIZONTAL_CAMERA 0",
                           "INVERT_HORIZONTAL_CAMERA 2"),
               unchanged, error),
           "reject invalid inversion value");
    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "CAMERA_SENSITIVITY 1.5", "CAMERA_SENSITIVITY 0.25"),
               unchanged, error),
           "reject sensitivity below minimum");
    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "CAMERA_SENSITIVITY 1.5", "CAMERA_SENSITIVITY 2.25"),
               unchanged, error),
           "reject sensitivity above maximum");
    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "CAMERA_SENSITIVITY 1.5", "CAMERA_SENSITIVITY nan"),
               unchanged, error),
           "reject non-finite sensitivity");
    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "END\n", ""), unchanged, error),
           "reject truncated preferences");
    Expect(!PreferencesCodec::Decode(
               ReplaceOnce(encoded, "END\n", "UNKNOWN_FIELD 1\nEND\n"), unchanged, error),
           "reject unknown preference fields");
    Expect(!PreferencesCodec::Decode(encoded + "TRAILING 1\n", unchanged, error),
           "reject data after preference terminator");

    AppPreferences optional;
    Expect(PreferencesCodec::Decode("POTLUCK_PREFERENCES 1\nEND\n", optional, error),
           "missing optional settings use defaults");
    Expect(!optional.invertHorizontalCamera && optional.cameraSensitivity == 1.0F,
           "omitted settings retain defaults");

    const auto testDirectory = std::filesystem::temp_directory_path() /
        ("potluck-preferences-tests-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto preferencesPath = testDirectory / "preferences.cfg";
    PreferencesService service(preferencesPath);
    std::string message;

    AppPreferences saved;
    saved.invertHorizontalCamera = true;
    saved.cameraSensitivity = 1.25F;
    Expect(service.Save(saved, message), "save preferences through filesystem service");

    AppPreferences loaded;
    Expect(service.Load(loaded, message), "load preferences through filesystem service");
    Expect(loaded.invertHorizontalCamera && loaded.cameraSensitivity == 1.25F,
           "filesystem preference round trip");

    saved.cameraSensitivity = 1.5F;
    Expect(service.Save(saved, message), "replace existing preferences file");
    Expect(service.Load(loaded, message), "load replaced preferences file");
    Expect(loaded.cameraSensitivity == 1.5F, "replacement stores newest preferences");

    AppPreferences invalid = saved;
    invalid.cameraSensitivity = std::numeric_limits<float>::infinity();
    Expect(!service.Save(invalid, message), "reject invalid preferences before writing");
    Expect(service.Load(loaded, message), "valid preferences survive rejected save");
    Expect(loaded.cameraSensitivity == 1.5F, "rejected save leaves existing file intact");

    {
        std::ofstream malformed(preferencesPath, std::ios::binary | std::ios::trunc);
        malformed << "POTLUCK_PREFERENCES 99\nEND\n";
    }
    loaded.invertHorizontalCamera = true;
    loaded.cameraSensitivity = 2.0F;
    Expect(!service.Load(loaded, message), "reject unsupported stored preferences");
    Expect(loaded.invertHorizontalCamera && loaded.cameraSensitivity == 2.0F,
           "rejected stored preferences leave output unchanged");
    Expect(std::filesystem::exists(preferencesPath.string() + ".rejected"),
           "rejected preferences are preserved");

    std::error_code cleanupError;
    std::filesystem::remove_all(testDirectory, cleanupError);
}
