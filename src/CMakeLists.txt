# indicator-desktop-shortcuts.h
# indicator-image-helper.h
# indicator-ng.h
# indicator-object.h
# indicator-service-manager.h
# indicator-service.h
# indicator.h

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-desktop-shortcuts.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-image-helper.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-ng.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-object.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-service-manager.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator-service.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/indicator.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/libayatana-indicator3-0.4/libayatana-indicator")

set(HEADERS
    indicator.h
    indicator-desktop-shortcuts.h
    indicator-image-helper.h
    indicator-object.h
    indicator-service.h
    indicator-service-manager.h
    indicator-ng.h
)

set(SOURCES
    gen-indicator-service.xml.c
    indicator-desktop-shortcuts.c
    indicator-image-helper.c
    indicator-ng.c
    indicator-object.c
    indicator-object-enum-types.c
    indicator-object-marshal.c
    indicator-service.c
    indicator-service-manager.c

)

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

# libayatana-indicator3.so

set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_library("ayatana-indicator3" SHARED ${SOURCES})
set_target_properties("ayatana-indicator3" PROPERTIES VERSION 7.0.0 SOVERSION 7)
target_compile_definitions("ayatana-indicator3" PUBLIC DG_LOG_DOMAIN="libayatana-indicator")
target_include_directories("ayatana-indicator3" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("ayatana-indicator3" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories("ayatana-indicator3" PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries("ayatana-indicator3" ${PROJECT_DEPS_LIBRARIES})
add_dependencies("ayatana-indicator3" "src-generated")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-indicator3.so" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-indicator3.so.7" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-indicator3.so.7.0.0" DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
