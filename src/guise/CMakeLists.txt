add_subdirectory(compiler)
add_subdirectory(debug)
add_subdirectory(vm)

set_target_properties(Compiler Debug VM PROPERTIES FOLDER "Libs")