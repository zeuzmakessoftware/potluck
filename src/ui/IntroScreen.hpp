#pragma once

namespace ultradope {

enum class IntroAction { None, Advance, Complete, Skip };

IntroAction DrawIntroScreen(int cardIndex);

}  // namespace ultradope
