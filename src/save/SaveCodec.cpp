#include "save/SaveCodec.hpp"

#include <iomanip>
#include <sstream>

namespace ultradope {
namespace {
template <typename Enum>
int EnumInt(Enum value) {
    return static_cast<int>(value);
}

bool InRange(int value, int maximum) { return value >= 0 && value <= maximum; }
bool ValidItem(int value) { return InRange(value, EnumInt(ItemType::SunspunFlower)); }
bool ValidCrop(int value) { return InRange(value, EnumInt(CropType::Sunspun)); }
bool ValidQuest(int value) { return InRange(value, EnumInt(QuestId::Complete)); }
bool ValidEra(int value) { return InRange(value, EnumInt(Era::NineteenSeventies)); }
bool ValidChapter(int value) { return InRange(value, EnumInt(ChapterId::VerticalSliceComplete)); }
bool ValidEnvironment(int value) { return InRange(value, EnumInt(EnvironmentId::HydroLab)); }
bool ValidEquipment(int value) { return InRange(value, EnumInt(HydroEquipment::GrowLight)); }

bool ValidProgression(const GameSession& session) {
    const auto& progression = session.progression;
    if (!progression.originCompleted) {
        if (progression.era != Era::Late1960s || progression.chapter != ChapterId::Origin ||
            progression.environment != EnvironmentId::OutdoorFarm || progression.hydroUnlocked ||
            progression.hydroBriefingSeen) {
            return false;
        }
    } else if (progression.era != Era::NineteenSeventies ||
               progression.chapter == ChapterId::Origin) {
        return false;
    }
    if (progression.environment == EnvironmentId::HydroLab && !progression.hydroUnlocked) {
        return false;
    }
    if (progression.hydroBriefingSeen && !progression.hydroUnlocked) return false;
    if ((progression.chapter == ChapterId::HydroponicsPrototype ||
         progression.chapter == ChapterId::VerticalSliceComplete) &&
        !progression.hydroUnlocked) {
        return false;
    }
    if (progression.hydroUnlocked && progression.chapter == ChapterId::OutdoorFoundations) {
        return false;
    }
    if (progression.lastMorningDoorCutsceneDay < 0 ||
        progression.lastMorningDoorCutsceneDay > session.calendar.day ||
        (!progression.originCompleted && progression.lastMorningDoorCutsceneDay != 0)) {
        return false;
    }
    return true;
}

bool ValidHydro(const GameSession& session) {
    const auto& hydro = session.hydro;
    if (hydro.selectedEquipment == HydroEquipment::None ||
        (hydro.completionAcknowledged && !hydro.completed)) {
        return false;
    }
    for (std::size_t index = 0; index < hydro.equipment.size(); ++index) {
        const HydroEquipment equipment = hydro.equipment[index];
        if (equipment != HydroEquipment::None &&
            !IsCompatibleHydroPlacement(static_cast<HydroSocket>(index), equipment)) {
            return false;
        }
    }
    if (hydro.completed) {
        if (!session.progression.hydroUnlocked || !ValidateHydroBuild(hydro).IsValid() ||
            session.quests.active != QuestId::Complete ||
            !session.quests.hydroBuildValidated ||
            session.progression.chapter != ChapterId::VerticalSliceComplete) {
            return false;
        }
    }
    if (session.quests.active == QuestId::Complete && !hydro.completed) return false;
    if ((session.quests.active == QuestId::HydroAssembly ||
         session.quests.active == QuestId::Complete) &&
        !session.progression.hydroUnlocked) {
        return false;
    }
    return true;
}

bool ValidValues(const GameSession& candidate) {
    if (candidate.calendar.day <= 0 || candidate.calendar.minuteOfDay < 0.0F ||
        candidate.calendar.minuteOfDay >= 1440.0F || candidate.money < 0 ||
        candidate.reputation < 0 || candidate.lifetimeRevenue < 0 ||
        candidate.player.maxEnergy <= 0 || candidate.player.energy < 0 ||
        candidate.player.energy > candidate.player.maxEnergy ||
        candidate.farm.wateringCanCapacity <= 0 || candidate.farm.wateringCan < 0 ||
        candidate.farm.wateringCan > candidate.farm.wateringCanCapacity) {
        return false;
    }
    for (int count : candidate.shipment.cropCounts) {
        if (count < 0) return false;
    }
    const auto& quest = candidate.quests;
    if (quest.tilled < 0 || quest.planted < 0 || quest.watered < 0 || quest.harvested < 0 ||
        quest.shipped < 0 || quest.salesCompleted < 0) {
        return false;
    }
    return ValidProgression(candidate) && ValidHydro(candidate);
}
}  // namespace

std::string SaveCodec::Encode(const GameSession& s) {
    std::ostringstream out;
    out << "POTLUCK 2\n";
    out << "META " << s.worldSeed << ' ' << s.calendar.day << ' ' << s.calendar.minuteOfDay
        << ' ' << EnumInt(s.calendar.weather) << '\n';
    out << "PLAYER " << s.player.position.x << ' ' << s.player.position.y << ' '
        << s.player.facingRadians << ' ' << s.player.energy << ' ' << s.player.maxEnergy << '\n';
    out << "ECON " << s.money << ' ' << s.reputation << ' ' << s.lifetimeRevenue << ' '
        << s.violetUnlocked << ' ' << s.sunspunUnlocked << '\n';
    out << "SELECT " << EnumInt(s.selectedTool) << ' ' << EnumInt(s.selectedCrop) << '\n';
    out << "WATER " << s.farm.wateringCan << ' ' << s.farm.wateringCanCapacity << '\n';
    for (const auto& slot : s.inventory.Slots()) {
        out << "ITEM " << EnumInt(slot.item) << ' ' << slot.count << '\n';
    }
    for (const auto& plot : s.farm.plots) {
        out << "PLOT " << plot.row << ' ' << plot.column << ' ' << plot.tilled << ' '
            << EnumInt(plot.crop) << ' ' << plot.growthDays << ' ' << plot.neglectDays << ' '
            << plot.wateredToday << ' ' << plot.withered << '\n';
    }
    out << "SHIP " << s.shipment.cropCounts[0] << ' ' << s.shipment.cropCounts[1] << ' '
        << s.shipment.cropCounts[2] << '\n';
    const auto& q = s.quests;
    out << "QUEST " << EnumInt(q.active) << ' ' << q.tilled << ' ' << q.planted << ' '
        << q.watered << ' ' << q.harvested << ' ' << q.shipped << ' ' << q.salesCompleted << ' '
        << q.spokeToTessa << ' ' << q.hydroBuildValidated << '\n';
    const auto& p = s.progression;
    out << "PROGRESSION " << EnumInt(p.era) << ' ' << EnumInt(p.chapter) << ' '
        << EnumInt(p.environment) << ' ' << p.originCompleted << ' ' << p.hydroUnlocked << ' '
        << p.hydroBriefingSeen << '\n';
    out << "MORNING_CUTSCENE " << p.lastMorningDoorCutsceneDay << '\n';
    out << "HYDRO " << EnumInt(s.hydro.selectedEquipment) << ' ' << s.hydro.completed << ' '
        << s.hydro.completionAcknowledged;
    for (HydroEquipment equipment : s.hydro.equipment) out << ' ' << EnumInt(equipment);
    out << '\n';
    out << "SUMMARY " << s.lastSummary.dayEnded << ' ' << s.lastSummary.itemsSold << ' '
        << s.lastSummary.revenue << ' ' << std::quoted(s.lastSummary.note) << '\n';
    out << "END\n";
    return out.str();
}

bool SaveCodec::Decode(std::string_view text, GameSession& output, std::string& error) {
    std::istringstream input{std::string(text)};
    std::string magic;
    int version = 0;
    if (!(input >> magic >> version) || magic != "POTLUCK" || (version != 1 && version != 2)) {
        error = "Unsupported or malformed Potluck save header.";
        return false;
    }

    GameSession candidate = GameSession::NewGame();
    candidate.inventory = Inventory{};
    std::size_t itemIndex = 0;
    std::size_t plotIndex = 0;
    bool foundMeta = false;
    bool foundPlayer = false;
    bool foundEconomy = false;
    bool foundSelection = false;
    bool foundWater = false;
    bool foundShipment = false;
    bool foundQuest = false;
    bool foundProgression = false;
    bool foundMorningCutscene = false;
    bool foundHydro = false;
    bool foundSummary = false;
    bool foundEnd = false;

    std::string key;
    while (input >> key) {
        if (key == "META") {
            int weather = 0;
            if (!(input >> candidate.worldSeed >> candidate.calendar.day >>
                  candidate.calendar.minuteOfDay >> weather) ||
                !InRange(weather, EnumInt(Weather::Rainy))) {
                break;
            }
            candidate.calendar.weather = static_cast<Weather>(weather);
            foundMeta = true;
        } else if (key == "PLAYER") {
            if (!(input >> candidate.player.position.x >> candidate.player.position.y >>
                  candidate.player.facingRadians >> candidate.player.energy >>
                  candidate.player.maxEnergy)) {
                break;
            }
            foundPlayer = true;
        } else if (key == "ECON") {
            if (!(input >> candidate.money >> candidate.reputation >> candidate.lifetimeRevenue >>
                  candidate.violetUnlocked >> candidate.sunspunUnlocked)) {
                break;
            }
            foundEconomy = true;
        } else if (key == "SELECT") {
            int tool = 0;
            int crop = 0;
            if (!(input >> tool >> crop) || !InRange(tool, EnumInt(Tool::Seeds)) ||
                !ValidCrop(crop)) {
                break;
            }
            candidate.selectedTool = static_cast<Tool>(tool);
            candidate.selectedCrop = static_cast<CropType>(crop);
            foundSelection = true;
        } else if (key == "WATER") {
            if (!(input >> candidate.farm.wateringCan >> candidate.farm.wateringCanCapacity)) break;
            foundWater = true;
        } else if (key == "ITEM") {
            int item = 0;
            int count = 0;
            if (!(input >> item >> count) || itemIndex >= Inventory::kSlotCount ||
                !ValidItem(item) || count < 0 || count > Inventory::kStackLimit) {
                break;
            }
            candidate.inventory.MutableSlots()[itemIndex++] = {static_cast<ItemType>(item), count};
        } else if (key == "PLOT") {
            int crop = 0;
            SoilPlot plot;
            if (!(input >> plot.row >> plot.column >> plot.tilled >> crop >> plot.growthDays >>
                  plot.neglectDays >> plot.wateredToday >> plot.withered) ||
                plotIndex >= candidate.farm.plots.size() || !ValidCrop(crop) ||
                plot.growthDays < 0 || plot.neglectDays < 0) {
                break;
            }
            plot.crop = static_cast<CropType>(crop);
            candidate.farm.plots[plotIndex++] = plot;
        } else if (key == "SHIP") {
            if (!(input >> candidate.shipment.cropCounts[0] >> candidate.shipment.cropCounts[1] >>
                  candidate.shipment.cropCounts[2])) {
                break;
            }
            foundShipment = true;
        } else if (key == "QUEST") {
            int quest = 0;
            auto& q = candidate.quests;
            if (!(input >> quest >> q.tilled >> q.planted >> q.watered >> q.harvested >>
                  q.shipped >> q.salesCompleted >> q.spokeToTessa >> q.hydroBuildValidated) ||
                !ValidQuest(quest)) {
                break;
            }
            q.active = static_cast<QuestId>(quest);
            foundQuest = true;
        } else if (key == "PROGRESSION") {
            int era = 0;
            int chapter = 0;
            int environment = 0;
            auto& progression = candidate.progression;
            if (!(input >> era >> chapter >> environment >> progression.originCompleted >>
                  progression.hydroUnlocked >> progression.hydroBriefingSeen) ||
                !ValidEra(era) || !ValidChapter(chapter) || !ValidEnvironment(environment)) {
                break;
            }
            progression.era = static_cast<Era>(era);
            progression.chapter = static_cast<ChapterId>(chapter);
            progression.environment = static_cast<EnvironmentId>(environment);
            foundProgression = true;
        } else if (key == "MORNING_CUTSCENE") {
            if (!(input >> candidate.progression.lastMorningDoorCutsceneDay)) break;
            foundMorningCutscene = true;
        } else if (key == "HYDRO") {
            int selected = 0;
            if (!(input >> selected >> candidate.hydro.completed >>
                  candidate.hydro.completionAcknowledged) ||
                selected < EnumInt(HydroEquipment::Reservoir) || !ValidEquipment(selected)) {
                break;
            }
            candidate.hydro.selectedEquipment = static_cast<HydroEquipment>(selected);
            bool placementsValid = true;
            for (std::size_t index = 0; index < candidate.hydro.equipment.size(); ++index) {
                int equipment = 0;
                if (!(input >> equipment) || !ValidEquipment(equipment)) {
                    placementsValid = false;
                    break;
                }
                candidate.hydro.equipment[index] = static_cast<HydroEquipment>(equipment);
            }
            if (!placementsValid) break;
            foundHydro = true;
        } else if (key == "SUMMARY") {
            if (!(input >> candidate.lastSummary.dayEnded >> candidate.lastSummary.itemsSold >>
                  candidate.lastSummary.revenue >> std::quoted(candidate.lastSummary.note))) {
                break;
            }
            foundSummary = true;
        } else if (key == "END") {
            foundEnd = true;
            break;
        } else {
            std::string ignored;
            std::getline(input, ignored);
        }
    }

    if (version == 1 && foundProgression) {
        candidate.progression.lastMorningDoorCutsceneDay =
            candidate.progression.originCompleted ? candidate.calendar.day : 0;
    }

    const bool complete = foundMeta && foundPlayer && foundEconomy && foundSelection && foundWater &&
                          itemIndex == Inventory::kSlotCount &&
                          plotIndex == candidate.farm.plots.size() && foundShipment && foundQuest &&
                          foundProgression && (version == 1 || foundMorningCutscene) && foundHydro &&
                          foundSummary && foundEnd;
    if (!complete || !ValidValues(candidate)) {
        error = "Potluck save data is truncated or contains invalid values.";
        return false;
    }

    output = candidate;
    error.clear();
    return true;
}

}  // namespace ultradope
