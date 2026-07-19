#pragma once

namespace ultradope {

enum class TitleAction { None, NewGame, Continue, Options, Quit };

bool LoadTitleScreenAssets();
void UnloadTitleScreenAssets();
void DrawTitleScreenBackdrop();
TitleAction DrawTitleScreen(bool hasSave);

}  // namespace ultradope
