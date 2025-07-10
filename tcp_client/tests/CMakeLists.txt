set (CMAKE_CXX_STANDARD 11)

include_directories(
  ./
  ../
)

add_executable(tcp_client_test
  tcp_client_test.cpp
  ../../common_tcp/common_tcp.c
  )

FILE (DOWNLOAD "https://raw.githubusercontent.com/meekrosoft/fff/v1.0/fff.h" "${CMAKE_CURRENT_SOURCE_DIR}/fff.h")
FILE (DOWNLOAD "https://raw.githubusercontent.com/doctest/doctest/v2.4.11/doctest/doctest.h" "${CMAKE_CURRENT_SOURCE_DIR}/doctest.h")

target_link_libraries(tcp_client_test PRIVATE tcp_client)

add_test(NAME tcp_client_test COMMAND tcp_client_test)

