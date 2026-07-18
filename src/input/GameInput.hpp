#pragma once

namespace ultradope {

struct GameInput {
    float moveRight = 0.0F;
    float moveForward = 0.0F;
    float cameraRotationRate = 0.0F;
    float cameraRotationDelta = 0.0F;
    float zoom = 0.0F;
    bool sprint = false;
    bool useTool = false;
    bool interact = false;
    bool refill = false;
    bool inventory = false;
    bool journal = false;
    bool pause = false;
    bool save = false;
    bool load = false;
    int toolSelection = -1;
    int seedCycle = 0;
};

GameInput PollGameInput(bool mouseCaptured);

}  // namespace ultradope
