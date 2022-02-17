if (FLAVOUR_GTK3)
    # libayatana-indicator3.so
    set (ayatana_indicator_gtkver "ayatana-indicator3")
else()
    # libayatana-indicator.so
    set (ayatana_indicator_gtkver "ayatana-indicator")
endif()

find_program(DBUS_TEST_RUNNER dbus-test-runner)

function(add_test_library_by_name name)
  set (TEST_LIBRARY_NAME ${name})
  add_library("${TEST_LIBRARY_NAME}" SHARED ${TEST_LIBRARY_NAME}.c)
  target_include_directories("${TEST_LIBRARY_NAME}" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
  target_include_directories("${TEST_LIBRARY_NAME}" PUBLIC "${CMAKE_SOURCE_DIR}/src")
  target_link_libraries("${TEST_LIBRARY_NAME}" ${PROJECT_DEPS_LIBRARIES} -l${ayatana_indicator_gtkver})
  target_link_directories("${TEST_LIBRARY_NAME}" PUBLIC "${CMAKE_BINARY_DIR}/src")
  add_dependencies("${TEST_LIBRARY_NAME}" "${ayatana_indicator_gtkver}")
endfunction()

function(add_test_executable_by_name name)
  set (TEST_EXECUTABLE_NAME ${name})
  add_executable("${TEST_EXECUTABLE_NAME}" ${TEST_EXECUTABLE_NAME}.c)
  target_compile_definitions("${TEST_EXECUTABLE_NAME}" PUBLIC BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
  target_compile_definitions("${TEST_EXECUTABLE_NAME}" PUBLIC SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
  target_include_directories("${TEST_EXECUTABLE_NAME}" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
  target_include_directories("${TEST_EXECUTABLE_NAME}" PUBLIC "${CMAKE_SOURCE_DIR}/src")
  target_link_libraries("${TEST_EXECUTABLE_NAME}" ${PROJECT_DEPS_LIBRARIES} -l${ayatana_indicator_gtkver})
  target_link_directories("${TEST_EXECUTABLE_NAME}" PUBLIC "${CMAKE_BINARY_DIR}/src")
  add_dependencies("${TEST_EXECUTABLE_NAME}" "${ayatana_indicator_gtkver}")
endfunction()

add_test_library_by_name(dummy-indicator-blank)
add_test_library_by_name(dummy-indicator-entry-func)
add_test_library_by_name(dummy-indicator-null)
add_test_library_by_name(dummy-indicator-signaler)
add_test_library_by_name(dummy-indicator-simple)
add_test_library_by_name(dummy-indicator-visible)

# session.conf
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/session.conf.in" "${CMAKE_CURRENT_BINARY_DIR}/session.conf" @ONLY)

# service-manager-connect.service
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/service-manager-connect.service.in" "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect.service" @ONLY)

# service-manager-connect
add_test_executable_by_name(service-manager-connect)

# service-manager-connect-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-tester"
    DEPENDS "service-manager-connect"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --dbus-config ${CMAKE_CURRENT_BINARY_DIR}/session.conf --task ${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect" >> "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-tester"
)
add_test("service-manager-connect-tester" "service-manager-connect-tester")

# service-manager-connect-service
add_test_executable_by_name(service-manager-connect-service)

# service-manager-no-connect
add_test_executable_by_name(service-manager-no-connect)

# service-manager-no-connect-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-manager-no-connect-tester"
    DEPENDS "service-manager-no-connect"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-manager-no-connect-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --dbus-config /usr/share/dbus-test-runner/session.conf --task ${CMAKE_CURRENT_BINARY_DIR}/service-manager-no-connect" >> "${CMAKE_CURRENT_BINARY_DIR}/service-manager-no-connect-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-manager-no-connect-tester"
)
add_test("service-manager-no-connect-tester" "service-manager-no-connect-tester")

# service-manager-nostart-connect
add_test_executable_by_name(service-manager-nostart-connect)

# service-manager-connect-nostart-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-nostart-tester"
    DEPENDS "service-manager-connect-service"
    DEPENDS "service-manager-nostart-connect"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-nostart-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --task ${CMAKE_CURRENT_BINARY_DIR}/service-manager-nostart-connect --task ${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-service" >> "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-nostart-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-manager-connect-nostart-tester"
)
add_test("service-manager-connect-nostart-tester" "service-manager-connect-nostart-tester")

# service-shutdown-timeout
add_test_executable_by_name(service-shutdown-timeout)

# service-shutdown-timeout-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-shutdown-timeout-tester"
    DEPENDS "service-shutdown-timeout"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-shutdown-timeout-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --dbus-config /usr/share/dbus-test-runner/session.conf --task ${CMAKE_CURRENT_BINARY_DIR}/service-shutdown-timeout" >> "${CMAKE_CURRENT_BINARY_DIR}/service-shutdown-timeout-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-shutdown-timeout-tester"
)
add_test("service-shutdown-timeout-tester" "service-shutdown-timeout-tester")

# service-version-bad.service
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/service-version-bad.service.in" "${CMAKE_CURRENT_BINARY_DIR}/service-version-bad.service" @ONLY)

# service-version-bad-service
add_test_executable_by_name(service-version-bad-service)

# service-version-good.service
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/service-version-good.service.in" "${CMAKE_CURRENT_BINARY_DIR}/service-version-good.service" @ONLY)

# service-version-good-service
add_test_executable_by_name(service-version-good-service)

# service-version-manager
add_test_executable_by_name(service-version-manager)

# service-version-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-version-tester"
    DEPENDS "service-shutdown-timeout"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-version-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --dbus-config ${CMAKE_CURRENT_BINARY_DIR}/session.conf --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-manager" >> "${CMAKE_CURRENT_BINARY_DIR}/service-version-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-version-tester"
)
add_test("service-version-tester" "service-version-tester")

# service-version-multiwatch-manager
add_test_executable_by_name(service-version-multiwatch-manager)

# service-version-multiwatch-manager-impolite
add_test_executable_by_name(service-version-multiwatch-manager-impolite)

# service-version-multiwatch-service
add_test_executable_by_name(service-version-multiwatch-service)

# service-version-multiwatch-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-tester"
    DEPENDS "service-version-multiwatch-manager"
    DEPENDS "service-version-multiwatch-manager-impolite"
    DEPENDS "service-version-multiwatch-service"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-tester"
    COMMAND
    echo "${DBUS_TEST_RUNNER} --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-service --task-name Service --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager --task-name Manager1 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager --task-name Manager2 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager --task-name Manager3 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager --task-name Manager4 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager --task-name Manager5 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager-impolite --task-name Impolite1 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager-impolite --task-name Impolite2 --task ${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-manager-impolite --task-name Impolite3" >> "${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/service-version-multiwatch-tester"
)
add_test("service-version-multiwatch-tester" "service-version-multiwatch-tester")

# test-desktop-shortcuts
add_test_executable_by_name(test-desktop-shortcuts)

# test-desktop-shortcuts-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts-tester"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/bash" > "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts-tester"
    COMMAND
    echo ". ${CMAKE_CURRENT_SOURCE_DIR}/run-xvfb.sh" >> "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts-tester"
    COMMAND
    echo "gtester -k --verbose -o=${CMAKE_CURRENT_BINARY_DIR}/loader-check-results.xml ${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts" >> "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/test-desktop-shortcuts-tester"
)
add_test("test-desktop-shortcuts-tester" "test-desktop-shortcuts-tester")

if (FLAVOUR_GTK3 AND ENABLE_IDO)
  # test-indicator-ng
  add_test_executable_by_name(test-indicator-ng)

  # org.ayatana.indicator.test.service
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/org.ayatana.indicator.test.service.in" "${CMAKE_CURRENT_BINARY_DIR}/org.ayatana.indicator.test.service" @ONLY)

  # test-indicator-ng-dbus-runner
  add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner"
      DEPENDS "test-indicator-ng"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
      COMMAND
      echo "#!/bin/sh" > "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner"
      COMMAND
      echo "${DBUS_TEST_RUNNER} --dbus-config /usr/share/dbus-test-runner/session.conf --task ${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng" >> "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner"
      COMMAND
      chmod +x "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner"
  )

  # test-indicator-ng-tester
  add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-tester"
      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
      COMMAND
      echo "#!/bin/bash" > "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-tester"
      COMMAND
      echo ". ${CMAKE_CURRENT_SOURCE_DIR}/run-xvfb.sh" >> "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-tester"
      COMMAND
      echo "gtester -k --verbose -o=${CMAKE_CURRENT_BINARY_DIR}/loader-check-results.xml ${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-dbus-runner" >> "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-tester"
      COMMAND
      chmod +x "${CMAKE_CURRENT_BINARY_DIR}/test-indicator-ng-tester"
  )
  add_test("test-indicator-ng-tester" "test-indicator-ng-tester")
endif(FLAVOUR_GTK3 AND ENABLE_IDO)

# test-loader
add_test_executable_by_name(test-loader)

# loader-tester
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/loader-tester"
    DEPENDS "test-loader"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    COMMAND
    echo "#!/bin/bash" > "${CMAKE_CURRENT_BINARY_DIR}/loader-tester"
    COMMAND
    echo ". ${CMAKE_CURRENT_SOURCE_DIR}/run-xvfb.sh" >> "${CMAKE_CURRENT_BINARY_DIR}/loader-tester"
    COMMAND
    echo "gtester -k --verbose -o=${CMAKE_CURRENT_BINARY_DIR}/loader-check-results.xml ${CMAKE_CURRENT_BINARY_DIR}/test-loader" >> "${CMAKE_CURRENT_BINARY_DIR}/loader-tester"
    COMMAND
    chmod +x "${CMAKE_CURRENT_BINARY_DIR}/loader-tester"
)
add_test("loader-tester" "loader-tester")

# indicator-test-service
add_executable("indicator-test-service" indicator-test-service.c)
target_include_directories("indicator-test-service" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_link_libraries("indicator-test-service" ${PROJECT_DEPS_LIBRARIES})

set (ALL_TESTERS
     "service-manager-connect-tester"
     "service-manager-no-connect-tester"
     "service-manager-connect-nostart-tester"
     "service-shutdown-timeout-tester"
     "service-version-tester"
     "service-version-multiwatch-tester"
     "test-desktop-shortcuts-tester"
     "loader-tester"
)

if (FLAVOUR_GTK3 AND ENABLE_IDO)
  set (ALL_TESTERS
       ${ALL_TESTERS}
       "test-indicator-ng-tester"
  )
endif()

add_custom_target("tests"
                  ALL
                  DEPENDS
                  ${ALL_TESTERS}
)
