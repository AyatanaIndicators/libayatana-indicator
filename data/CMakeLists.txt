# ayatana-indicator3-0.4.pc

if (FLAVOUR_GTK3)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator3.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.${API_VERSION}.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.${API_VERSION}.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/")
else()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator-0.${API_VERSION}.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator-0.${API_VERSION}.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/")
endif()

# ayatana-indicators.target (only required once, so let's do it with FLAVOUR_GTK3)

if (FLAVOUR_GTK3)

    pkg_check_modules(SYSTEMD systemd)

    if (${SYSTEMD_FOUND})
        if (${SYSTEMD_VERSION} VERSION_LESS 247)
            set(SYSTEMD_USER_UNIT_DIR ${CMAKE_INSTALL_PREFIX}/lib/systemd/user)
        else()
            pkg_get_variable(SYSTEMD_USER_UNIT_DIR systemd systemd_user_unit_dir)
        endif()
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicators.target" DESTINATION "${SYSTEMD_USER_UNIT_DIR}")
    endif()

endif()
