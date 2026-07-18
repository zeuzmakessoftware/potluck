#include "input/GameInput.hpp"

#include <raylib.h>

namespace ultradope {

GameInput PollGameInput(bool mouseCaptured) {
    GameInput input;
    input.moveRight = static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A));
    input.moveForward = static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S));
    input.cameraRotationRate = static_cast<float>(IsKeyDown(KEY_RIGHT)) -
                               static_cast<float>(IsKeyDown(KEY_LEFT));
    if (mouseCaptured) input.cameraRotationDelta = GetMouseDelta().x * 0.0032F;
    input.zoom = GetMouseWheelMove();
    input.sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    input.useTool = IsKeyPressed(KEY_F) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input.interact = IsKeyPressed(KEY_E);
    input.refill = IsKeyPressed(KEY_R);
    input.inventory = IsKeyPressed(KEY_I);
    input.journal = IsKeyPressed(KEY_J);
    input.pause = IsKeyPressed(KEY_ESCAPE);
    input.save = IsKeyPressed(KEY_F5);
    input.load = IsKeyPressed(KEY_F9);
    if (IsKeyPressed(KEY_ONE)) input.toolSelection = 0;
    if (IsKeyPressed(KEY_TWO)) input.toolSelection = 1;
    if (IsKeyPressed(KEY_THREE)) input.toolSelection = 2;
    if (IsKeyPressed(KEY_FOUR)) input.toolSelection = 3;
    if (IsKeyPressed(KEY_Z)) input.seedCycle = -1;
    if (IsKeyPressed(KEY_X)) input.seedCycle = 1;
    return input;
}

}  // namespace ultradope
