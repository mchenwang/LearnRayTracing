cmake_minimum_required (VERSION 3.18)
project (raytracer)

set(SOURCES
    src/PPMImage.cpp
    src/main.cpp
    src/BVH.cpp
    src/hittable.cpp
    src/RenderThreadPool.cpp
)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)

    function(enable_cxx_compiler_flag_if_supported flag)
        string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
        if(flag_already_set EQUAL -1)
            check_cxx_compiler_flag("${flag}" flag_supported)
            if(flag_supported)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            endif()
            unset(flag_supported CACHE)
        endif()
    endfunction()

    enable_cxx_compiler_flag_if_supported("-Wall")
    enable_cxx_compiler_flag_if_supported("-Wextra")
    enable_cxx_compiler_flag_if_supported("-pedantic")
    enable_cxx_compiler_flag_if_supported("-std=c++17")
    # enable_cxx_compiler_flag_if_supported("-pthread")
    enable_cxx_compiler_flag_if_supported("-O3")
    # enable_cxx_compiler_flag_if_supported("-fopenmp")
endif()

if (MSVC_VERSION GREATER_EQUAL "1914")
    add_compile_options("/Zc:__cplusplus") 
endif()
 
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++17" _compiler_supports_cxx17)
    if (_compiler_supports_cxx17)
        add_compile_options("/std:c++17")
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../target/)

# include_directories(${CMAKE_CURRENT_LIST_DIR}/header)

# configure_file(config/project_path.hpp.in ../config/project_path.hpp)
configure_file(
    "${PROJECT_SOURCE_DIR}/config/project_path.hpp.in"
    "${PROJECT_BINARY_DIR}/config/project_path.hpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/config
        ${PROJECT_SOURCE_DIR}/tools
        ${PROJECT_BINARY_DIR}/config
)