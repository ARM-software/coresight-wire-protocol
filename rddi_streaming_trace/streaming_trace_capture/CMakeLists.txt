include_directories(
  ./
  ${RDDI_INCLUDE_DIR}
  )


# trace capture test program
add_executable(streaming_trace_capture
  main.c
  )

if(CMAKE_VERSION VERSION_LESS "3.1")
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
  endif()
else()
  set(CMAKE_C_STANDARD 99)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(RDDI_VER_SUFFIX "_2")
endif()

target_link_libraries(streaming_trace_capture
  rddi_streaming_trace_amis${RDDI_VER_SUFFIX}
  ${Boost_LIBRARIES})
rpath_origin(streaming_trace_capture)

install(
  TARGETS streaming_trace_capture
  DESTINATION rddi_streaming_trace)
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/trace_config.xml
  DESTINATION rddi_streaming_trace)
