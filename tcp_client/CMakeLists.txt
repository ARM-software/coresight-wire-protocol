include_directories(
  ./
  ../common_client
  ../common_tcp
  ${Boost_INCLUDE_DIRS}
  )

set(src
  ../common_tcp/common_tcp.c
  ./tcp_device.cpp
  )

add_library(tcp_client STATIC ${src})
set_property(TARGET tcp_client PROPERTY POSITION_INDEPENDENT_CODE ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(tcp_client PRIVATE "-fvisibility=hidden")
endif ()

add_subdirectory(tests)

