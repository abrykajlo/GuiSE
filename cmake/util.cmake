macro(TOUCH_IF_NOT_EXISTS filename)
    if(NOT EXISTS ${filename})
        file(TOUCH ${filename})
    endif()
endmacro()

function(MAKE_SOURCES sources)
    cmake_parse_arguments(MAKE_SOURCES "" "" "H;H_CPP" ${ARGN})
    
    if(NOT MAKE_SOURCES_H OR NOT MAKE_SOURCES_H_CPP)
        message(FATAL_ERROR "missing filenames")
    endif()

    foreach(filename ${MAKE_SOURCES_H_CPP})
        set(cpp_file ${filename}.cpp)
        set(h_file ${filename}.h)

        TOUCH_IF_NOT_EXISTS(${cpp_file})
        TOUCH_IF_NOT_EXISTS(${h_file})

        list(APPEND ${sources} ${cpp_file} ${h_file})
    endforeach()
    
    foreach(filename ${MAKE_SOURCES_H})
        set(h_file ${filename}.h)

        TOUCH_IF_NOT_EXISTS(${h_file})

        list(APPEND ${sources} ${h_file})
    endforeach()

    set(${sources} ${${sources}} PARENT_SCOPE)
endfunction()