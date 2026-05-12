cmake_minimum_required(VERSION 3.28)
project(vvvv VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(vvvv main.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(vvvv PRIVATE Vulkan::Vulkan)

# Shader compile
find_program(SLANGC_EXE slangc REQUIRED)
set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/shader")
set(SHADER_BIN_DIR "${CMAKE_BINARY_DIR}/shader")
file(MAKE_DIRECTORY "${SHADER_BIN_DIR}")
set(SHADER_BINS)
function(compile_shader SHADER_NAME STAGE ENTRY)
    set(SHADER_SRC "${SHADER_SRC_DIR}/${SHADER_NAME}.slang")
    set(SHADER_BIN "${SHADER_BIN_DIR}/${SHADER_NAME}.${STAGE}.spv")

    set(SHADER_DEPS)
    foreach (DEP IN ITEMS ${ARGN})
        list(APPEND SHADER_DEPS "${SHADER_SRC_DIR}/${DEP}.slang")
    endforeach()

    add_custom_command(
        OUTPUT ${SHADER_BIN}
        COMMAND ${SLANGC_EXE} -target spirv -stage ${STAGE} -entry ${ENTRY} -o ${SHADER_BIN} ${SHADER_SRC}
        DEPENDS ${SHADER_SRC} ${SHADER_DEPS}
        COMMENT "Compiling shader ${SHADER_NAME} (${STAGE})"
        VERBATIM
    )

    list(APPEND SHADER_BINS ${SHADER_BIN})
    set(SHADER_BINS ${SHADER_BINS} PARENT_SCOPE)
endfunction()

add_custom_target(shaders ALL DEPENDS ${SHADER_BINS})
add_dependencies(vvvv shaders)
