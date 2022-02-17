if (FLAVOUR_GTK3 AND ENABLE_IDO)
    # ayatana-indicator-loader3

    add_executable("ayatana-indicator-loader3" indicator-loader.c)
    target_include_directories("ayatana-indicator-loader3" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
    target_include_directories("ayatana-indicator-loader3" PUBLIC "${CMAKE_SOURCE_DIR}/src")
    target_link_libraries("ayatana-indicator-loader3" ${PROJECT_DEPS_LIBRARIES} "-L${CMAKE_BINARY_DIR}/src" -layatana-indicator3)
    add_dependencies("ayatana-indicator-loader3" "ayatana-indicator3")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator-loader3" DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME}")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/80indicator-debugging" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")
endif()
