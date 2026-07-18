#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ultradope {

enum class HydroEquipment : std::uint8_t { None, Reservoir, Pump, GrowChannel, GrowLight };
enum class HydroSocket : std::uint8_t {
    ReservoirBase,
    LeftPump,
    RightPump,
    LeftChannel,
    CenterChannel,
    RightChannel,
    LeftLight,
    CenterLight,
    RightLight,
    Count
};

constexpr std::size_t kHydroSocketCount = static_cast<std::size_t>(HydroSocket::Count);
constexpr int kHydroBuildGrant = 650;
constexpr int kHydroAvailablePower = 500;

struct HydroState {
    std::array<HydroEquipment, kHydroSocketCount> equipment{};
    HydroEquipment selectedEquipment = HydroEquipment::Reservoir;
    bool completed = false;
    bool completionAcknowledged = false;
};

struct HydroOperationResult {
    bool success = false;
    std::string message;
};

struct HydroValidationResult {
    bool hasExactlyOneReservoir = false;
    bool hasExactlyOnePump = false;
    bool hasMinimumChannels = false;
    bool allChannelsLit = false;
    bool allChannelsReachable = false;
    bool withinBudget = false;
    bool withinPower = false;
    int totalCost = 0;
    int activePower = 0;
    std::vector<std::string> issues;

    bool IsValid() const;
};

int HydroEquipmentCost(HydroEquipment equipment);
int HydroEquipmentPower(HydroEquipment equipment);
bool IsCompatibleHydroPlacement(HydroSocket socket, HydroEquipment equipment);
HydroOperationResult PlaceHydroEquipment(HydroState& state, HydroSocket socket,
                                         HydroEquipment equipment);
HydroOperationResult RemoveHydroEquipment(HydroState& state, HydroSocket socket);
HydroValidationResult ValidateHydroBuild(const HydroState& state);

}  // namespace ultradope
