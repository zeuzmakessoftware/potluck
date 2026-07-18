#include "save/SaveService.hpp"

#include "save/SaveCodec.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace ultradope {
namespace {
std::string ReadAll(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}
}  // namespace

SaveService::SaveService() {
    const char* home = std::getenv("HOME");
    const std::filesystem::path root = home != nullptr ? home : ".";
    const auto directory = root / "Library" / "Application Support" / "Potluck" / "saves";
    savePath_ = directory / "slot1.save";
    backupPath_ = directory / "slot1.backup.save";
}

bool SaveService::HasSave() const { return std::filesystem::exists(savePath_); }

bool SaveService::Save(const GameSession& session, std::string& message) const {
    std::error_code error;
    std::filesystem::create_directories(savePath_.parent_path(), error);
    if (error) {
        message = "Could not create the save directory.";
        return false;
    }
    const auto temporary = savePath_.string() + ".tmp";
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        output << SaveCodec::Encode(session);
        if (!output.good()) {
            message = "Could not write the save file.";
            return false;
        }
    }
    if (std::filesystem::exists(savePath_)) {
        std::filesystem::copy_file(savePath_, backupPath_,
            std::filesystem::copy_options::overwrite_existing, error);
        error.clear();
    }
    std::filesystem::rename(temporary, savePath_, error);
    if (error) {
        std::filesystem::remove(savePath_, error);
        error.clear();
        std::filesystem::rename(temporary, savePath_, error);
    }
    if (error) {
        message = "Could not finalize the save file.";
        return false;
    }
    message = "Game saved.";
    return true;
}

bool SaveService::Load(GameSession& session, std::string& message) const {
    std::string error;
    GameSession loaded;
    if (std::filesystem::exists(savePath_) &&
        SaveCodec::Decode(ReadAll(savePath_), loaded, error)) {
        session = loaded;
        message = "Game loaded.";
        return true;
    }
    if (std::filesystem::exists(backupPath_) &&
        SaveCodec::Decode(ReadAll(backupPath_), loaded, error)) {
        session = loaded;
        message = "Primary save was damaged; backup restored.";
        return true;
    }
    message = error.empty() ? "No save found." : error;
    return false;
}

const std::filesystem::path& SaveService::SavePath() const { return savePath_; }

}  // namespace ultradope
