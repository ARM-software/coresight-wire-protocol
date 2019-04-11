project(streaming_trace)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

# core library
add_subdirectory(common   ${CMAKE_CURRENT_BINARY_DIR}/common) 

# rddi_streaming_trace_amis impl
add_subdirectory(rddi_streaming_trace_amis   ${CMAKE_CURRENT_BINARY_DIR}/rddi_streaming_trace_amis) 

# streaming trace capture tool
add_subdirectory(streaming_trace_capture  ${CMAKE_CURRENT_BINARY_DIR}/streaming_trace_capture) 
