#pragma once

namespace ultradope {

enum class TitleAction { None, NewGame, Continue, Quit };

TitleAction DrawTitleScreen(bool hasSave);

}  // namespace ultradope
