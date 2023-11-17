add_subdirectory(compiler)
add_subdirectory(vm)

set_target_properties(Compiler VM PROPERTIES FOLDER "Libs")