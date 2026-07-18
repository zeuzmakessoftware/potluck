#include "domain/Hydroponics.hpp"

namespace ultradope {
namespace {
std::size_t Index(HydroSocket socket) { return static_cast<std::size_t>(socket); }

bool IsValidSocket(HydroSocket socket) { return Index(socket) < kHydroSocketCount; }

HydroSocket MatchingLight(HydroSocket channel) {
    switch (channel) {
        case HydroSocket::LeftChannel: return HydroSocket::LeftLight;
        case HydroSocket::CenterChannel: return HydroSocket::CenterLight;
        case HydroSocket::RightChannel: return HydroSocket::RightLight;
        default: return HydroSocket::Count;
    }
}

void AddIssue(bool condition, std::vector<std::string>& issues, const char* message) {
    if (!condition) issues.emplace_back(message);
}
}  // namespace

bool HydroValidationResult::IsValid() const {
    return hasExactlyOneReservoir && hasExactlyOnePump && hasMinimumChannels &&
           allChannelsLit && allChannelsReachable && withinBudget && withinPower;
}

int HydroEquipmentCost(HydroEquipment equipment) {
    switch (equipment) {
        case HydroEquipment::Reservoir: return 120;
        case HydroEquipment::Pump: return 140;
        case HydroEquipment::GrowChannel: return 110;
        case HydroEquipment::GrowLight: return 75;
        case HydroEquipment::None: return 0;
    }
    return 0;
}

int HydroEquipmentPower(HydroEquipment equipment) {
    switch (equipment) {
        case HydroEquipment::Pump: return 120;
        case HydroEquipment::GrowLight: return 180;
        default: return 0;
    }
}

bool IsCompatibleHydroPlacement(HydroSocket socket, HydroEquipment equipment) {
    switch (socket) {
        case HydroSocket::ReservoirBase: return equipment == HydroEquipment::Reservoir;
        case HydroSocket::LeftPump:
        case HydroSocket::RightPump: return equipment == HydroEquipment::Pump;
        case HydroSocket::LeftChannel:
        case HydroSocket::CenterChannel:
        case HydroSocket::RightChannel: return equipment == HydroEquipment::GrowChannel;
        case HydroSocket::LeftLight:
        case HydroSocket::CenterLight:
        case HydroSocket::RightLight: return equipment == HydroEquipment::GrowLight;
        case HydroSocket::Count: return false;
    }
    return false;
}

HydroOperationResult PlaceHydroEquipment(HydroState& state, HydroSocket socket,
                                         HydroEquipment equipment) {
    if (!IsValidSocket(socket) || equipment == HydroEquipment::None ||
        !IsCompatibleHydroPlacement(socket, equipment)) {
        return {false, "That equipment does not fit this socket."};
    }
    if (state.completed) return {false, "The completed prototype is locked."};
    if (state.equipment[Index(socket)] != HydroEquipment::None) {
        return {false, "Remove the installed equipment first."};
    }
    state.equipment[Index(socket)] = equipment;
    return {true, "Equipment installed."};
}

HydroOperationResult RemoveHydroEquipment(HydroState& state, HydroSocket socket) {
    if (!IsValidSocket(socket)) return {false, "No socket selected."};
    if (state.completed) return {false, "The completed prototype is locked."};
    if (state.equipment[Index(socket)] == HydroEquipment::None) {
        return {false, "That socket is already empty."};
    }
    state.equipment[Index(socket)] = HydroEquipment::None;
    return {true, "Equipment removed."};
}

HydroValidationResult ValidateHydroBuild(const HydroState& state) {
    HydroValidationResult result;
    int reservoirs = 0;
    int pumps = 0;
    int channels = 0;
    HydroSocket pumpSocket = HydroSocket::Count;
    for (std::size_t index = 0; index < state.equipment.size(); ++index) {
        const HydroEquipment equipment = state.equipment[index];
        result.totalCost += HydroEquipmentCost(equipment);
        result.activePower += HydroEquipmentPower(equipment);
        if (equipment == HydroEquipment::Reservoir) ++reservoirs;
        if (equipment == HydroEquipment::Pump) {
            ++pumps;
            pumpSocket = static_cast<HydroSocket>(index);
        }
        if (equipment == HydroEquipment::GrowChannel) ++channels;
    }
    result.hasExactlyOneReservoir = reservoirs == 1;
    result.hasExactlyOnePump = pumps == 1;
    result.hasMinimumChannels = channels >= 2;
    result.allChannelsLit = true;
    result.allChannelsReachable = true;
    for (HydroSocket channel : {HydroSocket::LeftChannel, HydroSocket::CenterChannel,
                                HydroSocket::RightChannel}) {
        if (state.equipment[Index(channel)] != HydroEquipment::GrowChannel) continue;
        if (state.equipment[Index(MatchingLight(channel))] != HydroEquipment::GrowLight) {
            result.allChannelsLit = false;
        }
        const bool reachable = pumpSocket == HydroSocket::LeftPump
            ? channel == HydroSocket::LeftChannel || channel == HydroSocket::CenterChannel
            : pumpSocket == HydroSocket::RightPump
                ? channel == HydroSocket::CenterChannel || channel == HydroSocket::RightChannel
                : false;
        if (!reachable) result.allChannelsReachable = false;
    }
    result.withinBudget = result.totalCost <= kHydroBuildGrant;
    result.withinPower = result.activePower <= kHydroAvailablePower;
    AddIssue(result.hasExactlyOneReservoir, result.issues, "Install exactly one reservoir.");
    AddIssue(result.hasExactlyOnePump, result.issues, "Install exactly one pump.");
    AddIssue(result.hasMinimumChannels, result.issues, "Install at least two grow channels.");
    AddIssue(result.allChannelsReachable, result.issues, "All channels must be fed by the pump.");
    AddIssue(result.allChannelsLit, result.issues, "Every channel needs its matching light.");
    AddIssue(result.withinPower, result.issues, "Active power exceeds 500 W.");
    AddIssue(result.withinBudget, result.issues, "Build cost exceeds the $650 grant.");
    return result;
}

}  // namespace ultradope
