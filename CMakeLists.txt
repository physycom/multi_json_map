cmake_minimum_required (VERSION 2.8)
project (multi_json_map)
enable_language(CXX)
set (CMAKE_CXX_STANDARD 11)

set(JSONCONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/jsoncons/src)
if (MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
include_directories(${JSONCONS_INCLUDE_DIR})

add_executable (multi_json_map "${CMAKE_SOURCE_DIR}/src/multi_json_map.cpp")
install (TARGETS multi_json_map	DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
