#include "TestHarness.hpp"
#include "domain/Inventory.hpp"

using namespace ultradope;

void RunInventoryTests() {
    Inventory inventory;
    Expect(inventory.Add(ItemType::CinderleafSeed, 4), "add seeds");
    ExpectEqual(inventory.Count(ItemType::CinderleafSeed), 4, "seed count");
    Expect(inventory.Remove(ItemType::CinderleafSeed, 3), "remove seeds");
    ExpectEqual(inventory.Count(ItemType::CinderleafSeed), 1, "remaining seeds");
    Expect(!inventory.Remove(ItemType::CinderleafSeed, 2), "reject excessive removal");
    ExpectEqual(inventory.Count(ItemType::CinderleafSeed), 1, "failed removal is atomic");

    Inventory stacked;
    Expect(stacked.Add(ItemType::VioletCometFlower, 150), "split large stack");
    ExpectEqual(stacked.Count(ItemType::VioletCometFlower), 150, "split stack total");

    Inventory full;
    for (auto& slot : full.MutableSlots()) slot = {ItemType::CinderleafFlower, 99};
    Expect(!full.CanAdd(ItemType::SunspunSeed, 1), "full inventory has no room");
    Expect(!full.Add(ItemType::SunspunSeed, 1), "full inventory rejects add");
}
