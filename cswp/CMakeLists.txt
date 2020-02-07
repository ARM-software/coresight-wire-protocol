project(libcswp)

include_directories(${libcswp_SOURCE_DIR})

add_library(cswp_common
  cswp_buffer.c
  )
set_property(TARGET cswp_common PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(usb_transport)
add_subdirectory(tcp_transport)
add_subdirectory(tests)

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
         message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/cswp.doxygen)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
endif()
