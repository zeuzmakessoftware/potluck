#pragma once

#include "domain/GameSession.hpp"

#include <filesystem>
#include <string>

namespace ultradope {

class SaveService {
public:
    SaveService();

    bool HasSave() const;
    bool Save(const GameSession& session, std::string& message) const;
    bool Load(GameSession& session, std::string& message) const;
    const std::filesystem::path& SavePath() const;

private:
    std::filesystem::path savePath_;
    std::filesystem::path backupPath_;
};

}  // namespace ultradope
