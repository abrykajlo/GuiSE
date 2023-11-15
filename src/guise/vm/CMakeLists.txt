MAKE_SOURCES(GUISE_VM_SOURCES H_CPP byte_code object vm H opcode)

add_library(VM ${GUISE_VM_SOURCES})
target_include_directories(VM PUBLIC ${GUISE_INCLUDE_DIR})
set_target_properties(VM PROPERTIES CXX_STANDARD 17)