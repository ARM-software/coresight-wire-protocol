include_directories(
  ${libcswp_SOURCE_DIR}
  ${libcswp_SOURCE_DIR}/client
  ${libcswp_SOURCE_DIR}/../usb_client
  ${Boost_INCLUDE_DIRS}
  )

add_library(cswp_usb_transport STATIC
  cswp_usb_transport.cpp
  )
set_property(TARGET cswp_usb_transport PROPERTY POSITION_INDEPENDENT_CODE ON)

