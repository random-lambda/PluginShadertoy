cmake_minimum_required(VERSION 3.13)
project(Shadertoy)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./bin)

# source code
set(SOURCES
	dllmain.cpp
	Shadertoy.cpp

# libraries
	libs/json11/json11.cpp
	libs/pugixml/src/pugixml.cpp
	libs/imgui/imgui_draw.cpp
	libs/imgui/imgui_widgets.cpp
	libs/imgui/imgui.cpp
)

# cmake toolchain 
if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

# openssl
find_package(CURL 7.85 REQUIRED)

# create executable
add_library(Shadertoy SHARED ${SOURCES})

set_target_properties(Shadertoy PROPERTIES OUTPUT_NAME "plugin")
set_target_properties(Shadertoy PROPERTIES PREFIX "")

# include directories
target_include_directories(Shadertoy PRIVATE libs inc)

target_link_libraries(Shadertoy CURL::libcurl)

if (NOT MSVC)
	target_compile_options(Shadertoy PRIVATE -Wno-narrowing)
endif()
set_target_properties(Shadertoy PROPERTIES
    CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
include(CTest)
if(BUILD_TESTING AND UNIX)
    add_executable(import_collision_test tests/import_collision.cpp)
    target_link_libraries(import_collision_test PRIVATE dl)
    target_link_options(import_collision_test PRIVATE -rdynamic)
    add_test(NAME import_collision COMMAND import_collision_test $<TARGET_FILE:Shadertoy>)
    set_tests_properties(import_collision PROPERTIES ENVIRONMENT "SHADERTOY_API_KEY=")
endif()

# json11 predates modern toolchains and omits its fixed-width integer header.
if(NOT MSVC)
    set_source_files_properties(libs/json11/json11.cpp PROPERTIES COMPILE_OPTIONS "-include;cstdint")
endif()
