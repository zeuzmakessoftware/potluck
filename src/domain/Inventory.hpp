#pragma once

#include "domain/Types.hpp"

#include <array>
#include <cstddef>

namespace ultradope {

struct ItemStack {
    ItemType item = ItemType::None;
    int count = 0;
};

class Inventory {
public:
    static constexpr std::size_t kSlotCount = 12;
    static constexpr int kStackLimit = 99;

    bool Add(ItemType item, int count);
    bool Remove(ItemType item, int count);
    bool CanAdd(ItemType item, int count) const;
    int Count(ItemType item) const;
    const std::array<ItemStack, kSlotCount>& Slots() const;
    std::array<ItemStack, kSlotCount>& MutableSlots();

private:
    std::array<ItemStack, kSlotCount> slots_{};
};

}  // namespace ultradope
