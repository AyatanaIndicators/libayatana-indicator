cmake_minimum_required(VERSION 3.13)
project(libayatana-indicator C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Options

option(ENABLE_TESTS "Enable all tests and checks" OFF)
option(ENABLE_COVERAGE "Enable coverage reports (includes enabling all tests and checks)" OFF)
option(ENABLE_WERROR "Treat all build warnings as errors" OFF)

if(ENABLE_COVERAGE)
    set(ENABLE_TESTS ON)
    set(CMAKE_BUILD_TYPE "Coverage")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(ENABLE_WERROR)
    add_definitions("-Werror")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions("-Weverything")
else()
    add_definitions("-Wall")
endif()

# Check for prerequisites

option(FLAVOUR_GTK2 "Build against GTK+-2.0" OFF)
option(FLAVOUR_GTK3 "Build against GTK+-3.0" ON)
option(ENABLE_LOADER "Build Ayatana Indicator Loader" ON)
option(ENABLE_IDO "Enable IDO specific code" ON)

if (FLAVOUR_GTK2)
    set (FLAVOUR_GTK3 OFF)
    set (ENABLE_IDO OFF)
endif()
if (FLAVOUR_GTK3)
    set (FLAVOUR_GTK2 OFF)
endif()

set(DEPS
    glib-2.0>=2.37
    gmodule-2.0
    gio-unix-2.0
)
if (FLAVOUR_GTK3)
    set(DEPS
    ${DEPS}
    gtk+-3.0>=3.24
    )
endif()
if (FLAVOUR_GTK2)
    set(DEPS
    ${DEPS}
    gtk+-2.0>=2.18
    )
endif()

if (FLAVOUR_GTK3 AND ENABLE_IDO)
    set(DEPS
        ${DEPS}
        libayatana-ido3-0.4>=0.8.2
    )
endif()

find_package (PkgConfig REQUIRED)
pkg_check_modules(PROJECT_DEPS REQUIRED ${DEPS})

include(CheckLibraryExists)

CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIB_M)
if (HAVE_LIB_M)
    set(EXTRA_LIBS ${EXTRA_LIBS} m)
endif (HAVE_LIB_M)

# Set global variables

include(GNUInstallDirs)
set(ABI_VERSION "7")
set(API_VERSION "4")
set(PROJECT_VERSION "0.9.3")
set(PROJECT_NAME "libayatana-indicator")

# Make everything

add_subdirectory(src)
add_subdirectory(data)

if (ENABLE_LOADER)

    add_subdirectory(tools)

endif()

set (COVERAGE_TEST_EXECUTABLES
     "service-manager-connect"
     "service-manager-connect-service"
     "service-manager-no-connect"
     "service-manager-nostart-connect"
     "service-shutdown-timeout"
     "service-version-bad-service"
     "service-version-good-service"
     "service-version-manager"
     "service-version-multiwatch-manager"
     "service-version-multiwatch-manager-impolite"
     "service-version-multiwatch-service"
     "test-desktop-shortcuts"
     "test-loader"
)

if(FLAVOUR_GTK3 AND ENABLE_IDO)
  set (COVERAGE_TEST_EXECUTABLES
       ${COVERAGE_TEST_EXECUTABLES}
       "test-indicator-ng"
  )
endif()

if(ENABLE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)

    if (ENABLE_COVERAGE)
        find_package(CoverageReport)
        ENABLE_COVERAGE_REPORT(
            TARGETS "ayatana-indicator3"
            TESTS ${COVERAGE_TEST_EXECUTABLES}
            FILTER /usr/include ${CMAKE_BINARY_DIR}/*
        )
    endif()

endif()

# Display config info

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "GTK+-3.0 build: ${FLAVOUR_GTK3}")
message(STATUS "GTK+-2.0 build: ${FLAVOUR_GTK2}")
message(STATUS "Loader enabled ${ENABLE_LOADER}")
message(STATUS "IDO enabled: ${ENABLE_IDO}")
message(STATUS "Unit tests: ${ENABLE_TESTS}")
message(STATUS "Build with -Werror: ${ENABLE_WERROR}")
