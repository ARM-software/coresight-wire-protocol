include(../../RDDIStandardConfig.cmake)

project(rddi_streaming_trace_amis)

include_directories(    ./
                          ${RDDI_INCLUDE_DIR}
                        ../common
                        ../../usb_client/
                        ${Boost_INCLUDE_DIRS})

set(src                 ../common/api_stubs.cpp
                        ./streaming_trace_usb.cpp
                        ./streaming_trace_amis.cpp
                        )

add_definitions(-D_RDDI_EXPORT)

rddi_library_name(LIBNAME rddi_streaming_trace_amis)

add_library(${LIBNAME} SHARED ${src})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(${LIBNAME} PRIVATE "-fvisibility=hidden")
endif ()

# add library dependencies
target_link_libraries(${LIBNAME} PRIVATE
  streaming_trace_core usb_client
  ${PLAT_USB_LIBS} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} )

# winusb support is loaded at runtime on windows
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS "/DELAYLOAD:winusb.dll")
endif()

# apply common flags for RDDI implementations
rddi_library_properties(${LIBNAME})

# copy dependent libs to build dir
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(
    TARGETS ${LIBNAME}
    DESTINATION rddi_streaming_trace)
else()
  install(
    TARGETS ${LIBNAME}
    LIBRARY NAMELINK_SKIP
    DESTINATION rddi_streaming_trace)
endif()

