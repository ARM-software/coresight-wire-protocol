add_executable(cswp_test
  cswp_test.c
  cswp_buffer_test.c
  cswp_command_test.c
  cswp_server_test.c)

include_directories(
  ${libcswp_SOURCE_DIR}
  ${libcswp_SOURCE_DIR}/server
  ${libcswp_SOURCE_DIR}/client
  )

target_link_libraries(cswp_test cswp_common cswp_server cswp_client)

add_test(NAME cswp_test COMMAND cswp_test)
