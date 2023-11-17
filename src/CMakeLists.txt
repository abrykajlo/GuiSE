include(util)

set(GUISE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(guise)

add_executable(GuiSE main.cpp)
target_link_libraries(GuiSE Compiler VM)
set_target_properties(GuiSE
    PROPERTIES
        CXX_STANDARD 17
        VS_DEBUGGER_COMMAND_ARGUMENTS ${CMAKE_SOURCE_DIR}/tests/main.gs
)