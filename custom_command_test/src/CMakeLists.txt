cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(GLSL_VALIDATOR "/home/nc/vulkan/VulkanSDK/1.0.65.0/x86_64/bin/glslangValidator" CACHE FILEPATH "Path to glslangValidator executable")

add_executable(app
        Main.cpp
        resources/VertexShader.vert
        FragmentShader.frag)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CXXCOMPILEOPTIONS -Wall -ansi -pedantic -Wextra -Wshadow -Weffc++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CXXCOMPILEOPTIONS /W4 /std:c++latest)
endif()

set_target_properties(app PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_options(app PRIVATE ${CXXCOMPILEOPTIONS})

function(addCompileShaderCommand)
    cmake_parse_arguments(addCompileShaderCommand "" "SHADER" "" ${ARGN})

    if(NOT addCompileShaderCommand_SHADER)
        message(FATAL_ERROR "Shader file name should be provided")
    endif()

    add_custom_command(OUTPUT ${addCompileShaderCommand_SHADER}
            COMMAND ${GLSL_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/resources/${addCompileShaderCommand_SHADER} -o ${CMAKE_BINARY_DIR}/bin/${addCompileShaderCommand_SHADER}.spv
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/resources/${addCompileShaderCommand_SHADER}
            COMMENT "Compiling shader")
endfunction()

addCompileShaderCommand(SHADER VertexShader.vert)
addCompileShaderCommand(SHADER FragmentShader.frag)