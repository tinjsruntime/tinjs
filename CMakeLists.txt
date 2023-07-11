CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (tin)

set(CMAKE_CXX_STANDARD 17)
SET (tin_VERSION_MAJOR 1)
SET (tin_VERSION_MINOR 0)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(JAVASCRIPTCORE_LIBRARY REQUIRED javascriptcoregtk-4.0)
PKG_CHECK_MODULES(JSONCPP REQUIRED jsoncpp)

INCLUDE_DIRECTORIES(
  "/usr/include/webkitgtk-4.0",
  "/usr/include/jsoncpp"
)

add_executable (tin src/main.cpp)
target_link_libraries(tin javascriptcoregtk-4.0 curl jsoncpp)

install (TARGETS tin DESTINATION bin)