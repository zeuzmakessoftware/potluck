#include "TestHarness.hpp"
#include "domain/Hydroponics.hpp"

using namespace ultradope;

namespace {
void Install(HydroState& state, HydroSocket socket, HydroEquipment equipment) {
    Expect(PlaceHydroEquipment(state, socket, equipment).success, "compatible equipment installs");
}

HydroState ValidLeftBuild() {
    HydroState state;
    Install(state, HydroSocket::ReservoirBase, HydroEquipment::Reservoir);
    Install(state, HydroSocket::LeftPump, HydroEquipment::Pump);
    Install(state, HydroSocket::LeftChannel, HydroEquipment::GrowChannel);
    Install(state, HydroSocket::CenterChannel, HydroEquipment::GrowChannel);
    Install(state, HydroSocket::LeftLight, HydroEquipment::GrowLight);
    Install(state, HydroSocket::CenterLight, HydroEquipment::GrowLight);
    return state;
}
}  // namespace

void RunHydroponicsTests() {
    HydroState state;
    Expect(!PlaceHydroEquipment(state, HydroSocket::LeftPump, HydroEquipment::Reservoir).success,
           "incompatible placement rejected");
    Install(state, HydroSocket::ReservoirBase, HydroEquipment::Reservoir);
    Expect(RemoveHydroEquipment(state, HydroSocket::ReservoirBase).success, "removal succeeds");
    Expect(!RemoveHydroEquipment(state, HydroSocket::ReservoirBase).success, "empty removal rejected");
    Expect(!RemoveHydroEquipment(state, static_cast<HydroSocket>(99)).success,
           "out-of-range socket rejected");
    Expect(!ValidateHydroBuild(HydroState{}).IsValid(), "empty build fails validation");

    HydroState left = ValidLeftBuild();
    HydroValidationResult validation = ValidateHydroBuild(left);
    Expect(validation.IsValid(), "left and center build is valid");
    ExpectEqual(validation.totalCost, 630, "required build cost");
    ExpectEqual(validation.activePower, 480, "required build power");

    HydroState right;
    Install(right, HydroSocket::ReservoirBase, HydroEquipment::Reservoir);
    Install(right, HydroSocket::RightPump, HydroEquipment::Pump);
    Install(right, HydroSocket::CenterChannel, HydroEquipment::GrowChannel);
    Install(right, HydroSocket::RightChannel, HydroEquipment::GrowChannel);
    Install(right, HydroSocket::CenterLight, HydroEquipment::GrowLight);
    Install(right, HydroSocket::RightLight, HydroEquipment::GrowLight);
    Expect(ValidateHydroBuild(right).IsValid(), "center and right build is valid");

    HydroState missingFlow = ValidLeftBuild();
    Install(missingFlow, HydroSocket::RightChannel, HydroEquipment::GrowChannel);
    Install(missingFlow, HydroSocket::RightLight, HydroEquipment::GrowLight);
    Expect(!ValidateHydroBuild(missingFlow).allChannelsReachable, "unfed channel fails flow");

    HydroState missingLight = ValidLeftBuild();
    RemoveHydroEquipment(missingLight, HydroSocket::CenterLight);
    Expect(!ValidateHydroBuild(missingLight).allChannelsLit, "channel needs matching light");

    HydroState powerOverflow = ValidLeftBuild();
    Install(powerOverflow, HydroSocket::RightLight, HydroEquipment::GrowLight);
    Expect(!ValidateHydroBuild(powerOverflow).withinPower, "third light exceeds power");

    HydroState budgetOverflow = ValidLeftBuild();
    Install(budgetOverflow, HydroSocket::RightChannel, HydroEquipment::GrowChannel);
    Expect(!ValidateHydroBuild(budgetOverflow).withinBudget, "extra channel exceeds grant");
}
