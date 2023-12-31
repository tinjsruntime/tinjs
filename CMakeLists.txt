CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (tin)

set(CMAKE_CXX_STANDARD 17)
SET (tin_VERSION_MAJOR 1)
SET (tin_VERSION_MINOR 0)
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(JAVASCRIPTCORE_LIBRARY REQUIRED javascriptcoregtk-4.0)
PKG_CHECK_MODULES(JSONCPP REQUIRED jsoncpp)
PKG_CHECK_MODULES(CURL REQUIRED libcurl)
PKG_CHECK_MODULES(OPENSSL REQUIRED openssl)

INCLUDE_DIRECTORIES(
  "/usr/include/webkitgtk-4.0",
  "/usr/include/jsoncpp"
)

add_executable (tin src/main.cpp)
target_link_libraries(tin javascriptcoregtk-4.0 curl jsoncpp ${OPENSSL_LIBRARIES})

install (TARGETS tin DESTINATION bin)