# ayatana-indicator3-0.4.pc

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicator3-0.4.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.4.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayatana-indicator3-0.4.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/")

# ayatana-indicators.target

pkg_check_modules(SYSTEMD systemd)

if (${SYSTEMD_FOUND})
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/ayatana-indicators.target" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/systemd/user")
endif()
