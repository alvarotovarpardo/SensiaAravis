# clang_setup.cmake

# Configuración de clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    set_target_properties(
        SensiaAravis PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
else()
    message(WARNING "clang-tidy not found")
endif()

# Configuración de clang-format
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")
    
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/include/*.h"
    )

    add_custom_target(
        clang_format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on source files"
    )
else()
    message(WARNING "clang-format not found")
endif()