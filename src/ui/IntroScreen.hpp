#pragma once

namespace ultradope {

enum class IntroAction { None, Advance, Complete, Skip };

struct IntroPresentation {
    int cardIndex;
    float cardTime;
    float animationTime;
    float fadeOpacity;
    bool inputEnabled;
};

bool LoadIntroScreenAssets();
void UnloadIntroScreenAssets();
IntroAction DrawIntroScreen(const IntroPresentation& presentation);

}  // namespace ultradope
