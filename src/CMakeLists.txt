include(util)

MAKE_SOURCES(GUISE_SOURCES H_CPP byte_code scanner vm H opcode types)

add_executable(GuiSE main.cpp ${GUISE_SOURCES})
set_target_properties(GuiSE PROPERTIES CXX_STANDARD 17)