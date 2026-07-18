add_executable(potluck_tests
    TestMain.cpp
    InventoryTests.cpp
    FarmTests.cpp
    HydroponicsTests.cpp
    ProgressionTests.cpp
    SaveCodecTests.cpp
)
target_link_libraries(potluck_tests PRIVATE potluck_core)
target_include_directories(potluck_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
potluck_set_project_options(potluck_tests)
potluck_set_warnings(potluck_tests)
add_test(NAME potluck_tests COMMAND potluck_tests)
