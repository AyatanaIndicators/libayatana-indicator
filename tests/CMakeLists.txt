# dummy-indicator-blank

set_source_files_properties(dummy-indicator-blank.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-blank" SHARED dummy-indicator-blank.c)
target_include_directories("dummy-indicator-blank" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-blank" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# dummy-indicator-entry-func

set_source_files_properties(dummy-indicator-entry-func.h PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-entry-func" SHARED dummy-indicator-entry-func.c)
target_include_directories("dummy-indicator-entry-func" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-entry-func" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# dummy-indicator-null

set_source_files_properties(dummy-indicator-null.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-null" SHARED dummy-indicator-null.c)
target_include_directories("dummy-indicator-null" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-null" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# dummy-indicator-signaller

set_source_files_properties(dummy-indicator-signaler.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-signaler" SHARED dummy-indicator-signaler.c)
target_include_directories("dummy-indicator-signaler" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-signaler" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# dummy-indicator-simple

set_source_files_properties(dummy-indicator-simple.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-simple" SHARED dummy-indicator-simple.c)
target_include_directories("dummy-indicator-simple" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-simple" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# dummy-indicator-visible

set_source_files_properties(dummy-indicator-visible.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("dummy-indicator-visible" SHARED dummy-indicator-visible.c)
target_include_directories("dummy-indicator-visible" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("dummy-indicator-visible" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-manager-connect

set_source_files_properties(service-manager-connect.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-manager-connect" SHARED service-manager-connect.c)
target_include_directories("service-manager-connect" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-manager-connect" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-manager-connect-service

set_source_files_properties(service-manager-connect-service.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-manager-connect-service" SHARED service-manager-connect-service.c)
target_include_directories("service-manager-connect-service" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-manager-connect-service" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-manager-no-connect

set_source_files_properties(service-manager-no-connect.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-manager-no-connect" SHARED service-manager-no-connect.c)
target_include_directories("service-manager-no-connect" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-manager-no-connect" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-manager-nostart-connect

set_source_files_properties(service-manager-nostart-connect.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-manager-nostart-connect" SHARED service-manager-nostart-connect.c)
target_include_directories("service-manager-nostart-connect" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-manager-nostart-connect" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-shutdown-timeout

set_source_files_properties(service-shutdown-timeout.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-shutdown-timeout" SHARED service-shutdown-timeout.c)
target_include_directories("service-shutdown-timeout" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-shutdown-timeout" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-bad-service

set_source_files_properties(service-version-bad-service.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-bad-service" SHARED service-version-bad-service.c)
target_include_directories("service-version-bad-service" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-bad-service" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-good-service

set_source_files_properties(service-version-good-service.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-good-service" SHARED service-version-good-service.c)
target_include_directories("service-version-good-service" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-good-service" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-manager

set_source_files_properties(service-version-manager.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-manager" SHARED service-version-manager.c)
target_include_directories("service-version-manager" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-manager" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-multiwatch-manager

set_source_files_properties(service-version-multiwatch-manager.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-multiwatch-manager" SHARED service-version-multiwatch-manager.c)
target_include_directories("service-version-multiwatch-manager" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-multiwatch-manager" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-multiwatch-manager-impolite

set_source_files_properties(service-version-multiwatch-manager-impolite.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-multiwatch-manager-impolite" SHARED service-version-multiwatch-manager-impolite.c)
target_include_directories("service-version-multiwatch-manager-impolite" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-multiwatch-manager-impolite" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# service-version-multiwatch-service

set_source_files_properties(service-version-multiwatch-service.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("service-version-multiwatch-service" SHARED service-version-multiwatch-service.c)
target_include_directories("service-version-multiwatch-service" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("service-version-multiwatch-service" PUBLIC "${CMAKE_SOURCE_DIR}/src")

# test-desktop-shortcuts

set_source_files_properties(test-desktop-shortcuts.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("test-desktop-shortcuts" test-desktop-shortcuts.c)
target_compile_definitions("test-desktop-shortcuts" PUBLIC BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions("test-desktop-shortcuts" PUBLIC SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories("test-desktop-shortcuts" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("test-desktop-shortcuts" PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries("test-desktop-shortcuts" ${PROJECT_DEPS_LIBRARIES} -layatana-indicator3)
target_link_directories("test-desktop-shortcuts" PUBLIC "${CMAKE_BINARY_DIR}/src")
add_dependencies("test-desktop-shortcuts" "ayatana-indicator3")
add_test("test-desktop-shortcuts" "test-desktop-shortcuts")

# test-indicator-ng

set_source_files_properties(test-indicator-ng.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("test-indicator-ng" test-indicator-ng.c)
target_compile_definitions("test-indicator-ng" PUBLIC BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions("test-indicator-ng" PUBLIC SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories("test-indicator-ng" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("test-indicator-ng" PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries("test-indicator-ng" ${PROJECT_DEPS_LIBRARIES} -layatana-indicator3)
target_link_directories("test-indicator-ng" PUBLIC "${CMAKE_BINARY_DIR}/src")
add_dependencies("test-indicator-ng" "ayatana-indicator3")
add_test("test-indicator-ng" "test-indicator-ng")

# test-loader

set_source_files_properties(test-loader.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("test-loader" test-loader.c)
target_compile_definitions("test-loader" PUBLIC BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories("test-loader" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("test-loader" PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries("test-loader" ${PROJECT_DEPS_LIBRARIES} -layatana-indicator3)
target_link_directories("test-loader" PUBLIC "${CMAKE_BINARY_DIR}/src")
add_dependencies("test-loader" "ayatana-indicator3")
add_test("test-loader" "test-loader")
