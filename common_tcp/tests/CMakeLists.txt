add_executable(common_tcp_test
  common_tcp_test.c
  ../common_tcp.c
  )

FILE (DOWNLOAD "https://raw.githubusercontent.com/meekrosoft/fff/v1.0/fff.h" "${CMAKE_CURRENT_SOURCE_DIR}/fff.h")
FILE (DOWNLOAD "https://raw.githubusercontent.com/silentbicycle/greatest/v1.4.2/greatest.h" "${CMAKE_CURRENT_SOURCE_DIR}/greatest.h")

add_test(NAME common_tcp_test COMMAND common_tcp_test)

