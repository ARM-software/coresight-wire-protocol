
include_directories(    ./
                        ${RDDI_INCLUDE_DIR}
                        ${Boost_INCLUDE_DIRS})

set(src                 ./env_map.cpp
                        ./env.cpp
                        ./util.cpp
                        ./last_error.cpp
                        ./streaming_trace_base.cpp
                        )

add_library(streaming_trace_core STATIC ${src})
set_property(TARGET streaming_trace_core PROPERTY POSITION_INDEPENDENT_CODE ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(streaming_trace_core PRIVATE "-fvisibility=hidden")
endif ()
