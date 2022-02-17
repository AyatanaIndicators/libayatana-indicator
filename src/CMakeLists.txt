# indicator-desktop-shortcuts.h
# indicator-image-helper.h
# indicator-ng.h
# indicator-object.h
# indicator-service-manager.h
# indicator-service.h
# indicator.h

if (FLAVOUR_GTK3)
    # libayatana-indicator3.so
    set (ayatana_indicator_gtkver "ayatana-indicator3")
else()
    # libayatana-indicator.so
    set (ayatana_indicator_gtkver "ayatana-indicator")
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-desktop-shortcuts.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-image-helper.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-ng.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-object.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-service-manager.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-service.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lib${ayatana_indicator_gtkver}-0.${API_VERSION}/libayatana-indicator")

set(HEADERS
    indicator.h
    indicator-desktop-shortcuts.h
    indicator-image-helper.h
    indicator-object.h
    indicator-service.h
    indicator-service-manager.h
)

if (FLAVOUR_GTK3 AND ENABLE_IDO)
    set(HEADERS
        ${HEADERS}
        indicator-ng.h
    )
endif()

set(SOURCES
    gen-indicator-service.xml.c
    indicator-desktop-shortcuts.c
    indicator-image-helper.c
    indicator-object.c
    indicator-object-enum-types.c
    indicator-object-marshal.c
    indicator-service.c
    indicator-service-manager.c
)

if (FLAVOUR_GTK3 AND ENABLE_IDO)
    set(SOURCES
        ${SOURCES}
        indicator-ng.c
    )
endif()

# indicator-object-enum-types.h

find_program(GLIB_MKENUMS glib-mkenums)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_MKENUMS}
    --template indicator-object-enum-types.h.template
    ${HEADERS}
    --output="${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.h"
)

# indicator-object-enum-types.c

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.c"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_MKENUMS}
    --template indicator-object-enum-types.c.template
    ${HEADERS}
    --output="${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.c"
)

find_program(GLIB_GENMARSHAL glib-genmarshal)

# indicator-object-marshal.h

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-marshal.h"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-enum-types.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_GENMARSHAL}
    --prefix=_indicator_object_marshal
    --header indicator-object-marshal.list
    --quiet
    --output="${CMAKE_CURRENT_BINARY_DIR}/indicator-object-marshal.h"
)

# indicator-object-marshal.c

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-marshal.c"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/indicator-object-marshal.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_GENMARSHAL}
    --prefix=_indicator_object_marshal
    --body indicator-object-marshal.list
    --include-header=indicator-object-marshal.h
    --quiet
    --output="${CMAKE_CURRENT_BINARY_DIR}/indicator-object-marshal.c"
)

add_custom_target("src-generated" DEPENDS "indicator-object-marshal.c")

# gen-indicator-service.xml.h

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gen-indicator-service.xml.h" "extern const char * _indicator_service;")

# gen-indicator-service.xml.c

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/indicator-service.xml" GEN_INDICATOR_SERVICE_XML_C)
string(REPLACE "\"" "\\\"" GEN_INDICATOR_SERVICE_XML_C ${GEN_INDICATOR_SERVICE_XML_C})
string(REPLACE "\n" "\\n\"\n\"" GEN_INDICATOR_SERVICE_XML_C ${GEN_INDICATOR_SERVICE_XML_C})
string(REGEX REPLACE "\n\"$" "\n" GEN_INDICATOR_SERVICE_XML_C ${GEN_INDICATOR_SERVICE_XML_C})
string(PREPEND GEN_INDICATOR_SERVICE_XML_C "const char * _indicator_service = \n\"")
string(APPEND GEN_INDICATOR_SERVICE_XML_C "\;")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gen-indicator-service.xml.c" ${GEN_INDICATOR_SERVICE_XML_C})


# libayatana-indicator{,3}.so

add_library("${ayatana_indicator_gtkver}" SHARED ${SOURCES})
set_target_properties("${ayatana_indicator_gtkver}" PROPERTIES VERSION ${ABI_VERSION}.0.0 SOVERSION ${ABI_VERSION})
target_compile_definitions("${ayatana_indicator_gtkver}" PUBLIC DG_LOG_DOMAIN="libayatana-indicator")
target_include_directories("${ayatana_indicator_gtkver}" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("${ayatana_indicator_gtkver}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories("${ayatana_indicator_gtkver}" PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_options("${ayatana_indicator_gtkver}" PRIVATE "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/indicator.symbols")
target_link_libraries("${ayatana_indicator_gtkver}" ${PROJECT_DEPS_LIBRARIES} ${EXTRA_LIBS})
add_dependencies("${ayatana_indicator_gtkver}" "src-generated")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${ayatana_indicator_gtkver}.so" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${ayatana_indicator_gtkver}.so.${ABI_VERSION}" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${ayatana_indicator_gtkver}.so.${ABI_VERSION}.0.0" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
