add_custom_target(
  target__cswp_server ALL
  COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/cswp_server
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cswp_server/build/cswp_server ${CMAKE_CURRENT_BINARY_DIR}/cswp_server)
set_target_properties(
  target__cswp_server PROPERTIES OUTPUT_NAME cswp_server)

# Install target scripts
install(
  PROGRAMS
    ${CMAKE_CURRENT_SOURCE_DIR}/gadget_setup
  DESTINATION target)

# Install target apps
install(
  PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/cswp_server
  DESTINATION target)
