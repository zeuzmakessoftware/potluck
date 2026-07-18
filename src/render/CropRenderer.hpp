#pragma once

#include "domain/Farm.hpp"

#include <raylib.h>

namespace ultradope {

void DrawCrop(const SoilPlot& plot, Vector3 position, std::size_t plotIndex);

}  // namespace ultradope
