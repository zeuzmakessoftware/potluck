#include <exception>
#include <iostream>

void RunInventoryTests();
void RunFarmTests();
void RunHydroponicsTests();
void RunProgressionTests();
void RunSaveCodecTests();

int main() {
    try {
        RunInventoryTests();
        RunFarmTests();
        RunHydroponicsTests();
        RunProgressionTests();
        RunSaveCodecTests();
        std::cout << "All Potluck tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}
