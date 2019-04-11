include_directories(
  ./
  ${Boost_INCLUDE_DIRS}
  ${PLAT_USB_INCLUDE_DIRS}
  )

set(src
  ./usb_device.cpp
  )

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

  set(plat_src            ./usb_device_windows.cpp)
    
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

  set(plat_src            ./usb_device_linux.cpp)

endif()

add_library(usb_client STATIC ${src} ${plat_src})
set_property(TARGET usb_client PROPERTY POSITION_INDEPENDENT_CODE ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(usb_client PRIVATE "-fvisibility=hidden")
endif ()
