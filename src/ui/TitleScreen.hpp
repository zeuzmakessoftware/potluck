#pragma once

namespace ultradope {

enum class TitleAction { None, NewGame, Continue, Options, Quit };

bool LoadTitleScreenAssets();
void UnloadTitleScreenAssets();
void DrawTitleScreenBackdrop(float animationTime);
TitleAction DrawTitleScreen(bool hasSave, float animationTime);

}  // namespace ultradope
