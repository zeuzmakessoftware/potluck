#pragma once

#include <raylib.h>

namespace ultradope::palette {

// Dusty, high-contrast seventies palette.  The deliberately restricted gamut
// helps the procedural geometry read like hand-authored PS2-era environment art.
inline constexpr Color Grass{72, 91, 46, 255};
inline constexpr Color GrassDark{42, 61, 34, 255};
inline constexpr Color Soil{91, 55, 33, 255};
inline constexpr Color WetSoil{47, 37, 28, 255};
inline constexpr Color Path{157, 112, 62, 255};
inline constexpr Color Cream{226, 210, 169, 255};
inline constexpr Color Ink{18, 18, 14, 255};
inline constexpr Color Leaf{47, 104, 48, 255};
inline constexpr Color LeafLight{83, 137, 59, 255};
inline constexpr Color Violet{112, 73, 128, 255};
inline constexpr Color Gold{231, 169, 35, 255};
inline constexpr Color Panel{12, 13, 10, 238};
inline constexpr Color Water{48, 107, 121, 255};
inline constexpr Color Wood{82, 51, 31, 255};
inline constexpr Color Roof{119, 49, 31, 255};

}  // namespace ultradope::palette
