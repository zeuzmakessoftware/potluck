#pragma once

#include <cstdint>

namespace ultradope {

enum class Era : std::uint8_t { Late1960s, NineteenSeventies };
enum class ChapterId : std::uint8_t {
    Origin,
    OutdoorFoundations,
    HydroponicsPrototype,
    VerticalSliceComplete
};
enum class EnvironmentId : std::uint8_t { OutdoorFarm, HydroLab };

struct ProgressionState {
    Era era = Era::Late1960s;
    ChapterId chapter = ChapterId::Origin;
    EnvironmentId environment = EnvironmentId::OutdoorFarm;
    bool originCompleted = false;
    bool hydroUnlocked = false;
    bool hydroBriefingSeen = false;
    int lastMorningDoorCutsceneDay = 0;
};

}  // namespace ultradope
