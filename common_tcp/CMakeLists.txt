include_directories(
  ./
  )

set(src
  ./common_tcp.c
  )

add_library(common_tcp STATIC ${src})
set_property(TARGET common_tcp PROPERTY POSITION_INDEPENDENT_CODE ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(common_tcp PRIVATE "-fvisibility=hidden")
endif ()

add_subdirectory(tests)

