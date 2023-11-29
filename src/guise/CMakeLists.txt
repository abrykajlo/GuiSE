set(GUISE_COMMON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/debug.h)

add_subdirectory(compiler)
add_subdirectory(vm)

set_target_properties(Compiler VM PROPERTIES FOLDER "Libs")