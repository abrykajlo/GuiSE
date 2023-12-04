MAKE_SOURCES(GUISE_COMPILER_SOURCES
    H_CPP binding compiler disassembler parser scanner types
)

add_library(Compiler ${GUISE_COMPILER_SOURCES} ${GUISE_COMMON_HEADERS})
target_include_directories(Compiler PUBLIC ${GUISE_INCLUDE_DIR})
set_target_properties(Compiler PROPERTIES CXX_STANDARD 17)