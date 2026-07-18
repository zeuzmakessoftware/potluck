#include "domain/GameSession.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace ultradope {

GameSession GameSession::NewGame() {
    GameSession session;
    session.farm = CreateFarm();
    session.inventory.Add(ItemType::CinderleafSeed, 4);
    return session;
}

const char* WeekdayName(int day) {
    static constexpr const char* kNames[] = {
        "Moonday", "Sproutday", "Midweek", "Thirstday",
        "Flowerday", "Sunrest", "Marketday"
    };
    const int index = std::max(0, day - 1) % 7;
    return kNames[index];
}

std::string FormatTime(float minuteOfDay) {
    const int total = std::clamp(static_cast<int>(minuteOfDay), 0, 1439);
    const int hour24 = total / 60;
    const int minute = total % 60;
    const int hour12 = hour24 % 12 == 0 ? 12 : hour24 % 12;
    std::ostringstream stream;
    stream << hour12 << ':' << std::setw(2) << std::setfill('0') << minute
           << (hour24 < 12 ? " AM" : " PM");
    return stream.str();
}

}  // namespace ultradope
