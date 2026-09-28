cmake_minimum_required(VERSION 3.16)
project(RakBotEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(rakbot ${SOURCES})

if(WIN32)
    target_link_libraries(rakbot PRIVATE ws2_32)
    target_compile_definitions(rakbot PRIVATE _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN)
elseif(UNIX)
    target_link_libraries(rakbot PRIVATE pthread)
endif()

set_target_properties(rakbot PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
