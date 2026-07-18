#include "domain/Inventory.hpp"

#include <algorithm>

namespace ultradope {

bool Inventory::CanAdd(ItemType item, int count) const {
    if (item == ItemType::None || count <= 0) return false;
    int room = 0;
    for (const auto& slot : slots_) {
        if (slot.item == item) room += kStackLimit - slot.count;
        if (slot.item == ItemType::None) room += kStackLimit;
    }
    return room >= count;
}

bool Inventory::Add(ItemType item, int count) {
    if (!CanAdd(item, count)) return false;
    for (auto& slot : slots_) {
        if (slot.item != item || slot.count >= kStackLimit) continue;
        const int moved = std::min(count, kStackLimit - slot.count);
        slot.count += moved;
        count -= moved;
    }
    for (auto& slot : slots_) {
        if (count == 0) break;
        if (slot.item != ItemType::None) continue;
        const int moved = std::min(count, kStackLimit);
        slot = {item, moved};
        count -= moved;
    }
    return true;
}

bool Inventory::Remove(ItemType item, int count) {
    if (item == ItemType::None || count <= 0 || Count(item) < count) return false;
    for (auto& slot : slots_) {
        if (slot.item != item) continue;
        const int moved = std::min(count, slot.count);
        slot.count -= moved;
        count -= moved;
        if (slot.count == 0) slot = {};
        if (count == 0) break;
    }
    return true;
}

int Inventory::Count(ItemType item) const {
    int total = 0;
    for (const auto& slot : slots_) {
        if (slot.item == item) total += slot.count;
    }
    return total;
}

const std::array<ItemStack, Inventory::kSlotCount>& Inventory::Slots() const { return slots_; }
std::array<ItemStack, Inventory::kSlotCount>& Inventory::MutableSlots() { return slots_; }

}  // namespace ultradope
