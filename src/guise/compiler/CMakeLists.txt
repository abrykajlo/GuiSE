MAKE_SOURCES(GUISE_COMPILER_SOURCES
    H_CPP compiler scanner types
    H backends/backend
)

add_library(Compiler ${GUISE_COMPILER_SOURCES})
target_include_directories(Compiler PUBLIC ${GUISE_INCLUDE_DIR})
set_target_properties(Compiler PROPERTIES CXX_STANDARD 17)