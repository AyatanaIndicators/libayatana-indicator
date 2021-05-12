cmake_minimum_required(VERSION 3.13)
project(libayatana-indicator C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

option(ENABLE_TESTS "Enable all tests and checks" OFF)
option(ENABLE_COVERAGE "Enable coverage reports (includes enabling all tests and checks)" OFF)

if(ENABLE_COVERAGE)
    set(ENABLE_TESTS ON)
    set(CMAKE_BUILD_TYPE "Coverage")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Check for prerequisites

set(DEPS glib-2.0>=2.58 gtk+-3.0>=3.24 libayatana-ido3-0.4>=0.8.2)

find_package (PkgConfig REQUIRED)
pkg_check_modules(PROJECT_DEPS REQUIRED ${DEPS})

# Set global variables

include(GNUInstallDirs)
set(ABI_VERSION "7")
set(API_VERSION "4")
set(PROJECT_VERSION "0.8.4")
set(PROJECT_NAME "libayatana-indicator")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Weverything")
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wno-c++98-compat -Wno-padded")
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wno-documentation")

else()

    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall")

endif()

# Make everything

add_subdirectory(src)
add_subdirectory(data)

if(ENABLE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tools)
    add_subdirectory(tests)

    if (ENABLE_COVERAGE)
        find_package(CoverageReport)
        ENABLE_COVERAGE_REPORT(
            TARGETS "ayatana-indicator3"
            TESTS "test-desktop-shortcuts" "test-indicator-ng" "test-loader"
            FILTER /usr/include ${CMAKE_BINARY_DIR}/*
        )
    endif()

endif()

# Display config info

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Unit tests: ${ENABLE_TESTS}")
