# ayatana-indicator-loader3

set_source_files_properties(indicator-loader.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("ayatana-indicator-loader3" indicator-loader.c)
target_include_directories("ayatana-indicator-loader3" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("ayatana-indicator-loader3" PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries("ayatana-indicator-loader3" ${PROJECT_DEPS_LIBRARIES} "-L${CMAKE_BINARY_DIR}/src" -layatana-indicator3)
add_dependencies("ayatana-indicator-loader3" "ayatana-indicator3")
