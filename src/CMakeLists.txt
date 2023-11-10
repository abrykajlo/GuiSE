include(util)

MAKE_SOURCES(GUISE_SOURCES H_CPP byte_code compiler debug scanner types vm H opcode)

add_executable(GuiSE main.cpp ${GUISE_SOURCES})
set_target_properties(GuiSE PROPERTIES CXX_STANDARD 17)