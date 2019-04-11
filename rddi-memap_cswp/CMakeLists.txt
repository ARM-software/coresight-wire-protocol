
project(rddi-memap_cswp)

include(../RDDIStandardConfig.cmake)

include_directories(    ./
                        ${RDDI_INCLUDE_DIR}
                        ../cswp
                        ../cswp/client
                        ../cswp/usb_transport
                        ${Boost_INCLUDE_DIRS}                        
                        )

set(src                 ./rddi_stubs.cpp
                        ./env_map.cpp
                        ./env.cpp
                        ./utils.cpp
                        ./rddi_error.cpp
                        ./rddi_logger.cpp
                        )

rddi_library_name(LIBNAME rddi-memap_cswp)

add_definitions(-DUSE_BOOST)

add_library(${LIBNAME} SHARED ${src})

# add library dependencies
target_link_libraries(${LIBNAME} PRIVATE
  cswp_client cswp_common cswp_usb_transport usb_client ${PLAT_USB_LIBS}
  ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} )

# apply common flags for RDDI implementations
rddi_library_properties(${LIBNAME})

# copy libs to build dir
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(
    TARGETS ${LIBNAME}
    DESTINATION rddi_mem-ap)
else()
  install(
    TARGETS ${LIBNAME}
    LIBRARY NAMELINK_SKIP
    DESTINATION rddi_mem-ap)
endif()
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/debug_config.xml
  DESTINATION rddi_mem-ap)