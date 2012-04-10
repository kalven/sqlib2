cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

pkg_check_modules(SQLITE REQUIRED sqlite3)

include_directories(${SQLITE_INCLUDE_DIRS})
include_directories(".")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x -Wall")
endif()

add_subdirectory(tests)
