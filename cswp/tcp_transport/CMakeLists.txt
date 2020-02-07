include_directories(
  ${libcswp_SOURCE_DIR}
  ${libcswp_SOURCE_DIR}/client
  ${libcswp_SOURCE_DIR}/../tcp_client
  ${libcswp_SOURCE_DIR}/../common_client
  ${Boost_INCLUDE_DIRS}
  )

add_library(cswp_tcp_transport STATIC
  cswp_tcp_transport.cpp
  )
set_property(TARGET cswp_tcp_transport PROPERTY POSITION_INDEPENDENT_CODE ON)

