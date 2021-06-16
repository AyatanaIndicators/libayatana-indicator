# ayatana-indicator3-0.4.pc

if (FLAVOUR_GTK3)
    if (ENABLE_IDO)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator3.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.${API_VERSION}.pc" @ONLY)
    else()
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator3-no-IDO.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.${API_VERSION}.pc" @ONLY)
    endif()
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.${API_VERSION}.pc" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig/")
else()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator-0.${API_VERSION}.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator-0.${API_VERSION}.pc" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig/")
endif()
