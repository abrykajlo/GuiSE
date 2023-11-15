MAKE_SOURCES(GUISE_DEBUG_SOURCES H_CPP debug)

add_library(Debug ${GUISE_DEBUG_SOURCES})
target_include_directories(Debug PUBLIC ${GUISE_INCLUDE_DIR})
set_target_properties(Debug PROPERTIES CXX_STANDARD 17)