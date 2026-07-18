cmake_minimum_required(VERSION 3.20)
project(Potluck VERSION 0.1.0 LANGUAGES CXX)

option(POTLUCK_BUILD_TESTS "Build Potluck tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(ProjectOptions)

add_library(potluck_core STATIC
    src/domain/Inventory.cpp
    src/domain/Farm.cpp
    src/domain/Hydroponics.cpp
    src/domain/GameSession.cpp
    src/systems/FarmSystem.cpp
    src/systems/DaySystem.cpp
    src/systems/EconomySystem.cpp
    src/systems/QuestSystem.cpp
    src/systems/HydroponicsSystem.cpp
    src/preferences/PreferencesCodec.cpp
    src/preferences/PreferencesService.cpp
    src/save/SaveCodec.cpp
)
target_include_directories(potluck_core PUBLIC src)
potluck_set_project_options(potluck_core)
potluck_set_warnings(potluck_core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)

add_executable(potluck
    src/main.cpp
    src/app/GameApp.cpp
    src/camera/ThirdPersonCamera.cpp
    src/input/GameInput.cpp
    src/render/PrimitiveDraw.cpp
    src/render/CropRenderer.cpp
    src/render/CharacterRenderer.cpp
    src/render/WorldRenderer.cpp
    src/render/HydroRenderer.cpp
    src/ui/UiDraw.cpp
    src/ui/Hud.cpp
    src/ui/TitleScreen.cpp
    src/ui/IntroScreen.cpp
    src/ui/ModalScreens.cpp
    src/world/FarmLayout.cpp
    src/world/HydroLayout.cpp
    src/save/SaveService.cpp
)
target_link_libraries(potluck PRIVATE potluck_core PkgConfig::RAYLIB)
target_include_directories(potluck PRIVATE src)
add_custom_command(TARGET potluck POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:potluck>/assets"
)
potluck_set_project_options(potluck)
potluck_set_warnings(potluck)

if(APPLE)
    target_link_libraries(potluck PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()

if(POTLUCK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
