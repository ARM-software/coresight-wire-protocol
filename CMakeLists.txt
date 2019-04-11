cmake_minimum_required (VERSION 2.8.12)

project(amis_poc)
enable_testing()

# use static libstdc++
if ( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libstdc++ ${CMAKE_CSS_STANDARD_LIBRARIES}")
endif ( CMAKE_COMPILER_IS_GNUCC )

# enable docs build
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Generate API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

# increase warning levels
if ( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif ( CMAKE_COMPILER_IS_GNUCC )
if ( MSVC )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif ( MSVC )


# Setup platform specific variables
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(PLATFORM_OS win_32)
  set(PLATFORM_COMPILER cl-16.00.40219.01)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(PLATFORM_OS linux)
  set(PLATFORM_COMPILER gcc-4.8.3)
else()
 error("${CMAKE_SYSTEM_NAME} not supported")
endif()

# PLATFORM_ARCH set on command line

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(SHORT_BUILD_TYPE dbg)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(SHORT_BUILD_TYPE rel)
else()
  error("Unknown build type ${CMAKE_BUILD_TYPE}")
endif()

set(PLATFORM ${PLATFORM_OS}-${PLATFORM_ARCH})

# find platform libraries
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  INCLUDE(FindPkgConfig)
  pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

  set(PLAT_USB_LIBS ${LIBUSB_LIBRARIES})
  set(PLAT_USB_INCLUDE_DIRS ${LIBUSB_INCLUDE_DIRS})
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(PLAT_USB_LIBS winusb setupapi)
endif()
Message("Platform USB include dirs: ${PLAT_USB_INCLUDE_DIRS}")
Message("Platform USB libraries: ${PLAT_USB_LIBS}")


# find boost libraries
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_definitions(-DBOOST_ALL_NO_LIB)
endif()
set(Boost_REALPATH 1)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread system chrono REQUIRED)

# find RDDI headers
find_path(RDDI_INCLUDE_DIR rddi.h
  HINTS ${RDDI_INCLUDE_DIR}
    ${amis_poc_SOURCE_DIR}/rddi
  PATH_SUFFIXES public/include)
message("Using RDDI headers: ${RDDI_INCLUDE_DIR}")

# set RPATH to point to $ORIGIN (same directory as target)
macro(rpath_origin TGT)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # set linker flags
    #   ensure RPATH points to same dir as lib
    set(linkFlags "-Wl,-rpath,$ORIGIN")
    set_target_properties(${TGT} PROPERTIES LINK_FLAGS "${linkFlags}")
    set_target_properties(${TGT} PROPERTIES SKIP_BUILD_RPATH true)
  endif()
endmacro()

add_subdirectory(usb_client)
add_subdirectory(rddi_streaming_trace)
add_subdirectory(cswp)
add_subdirectory(rddi-memap_cswp)

# Build target apps on linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
add_subdirectory(target)
endif()

# install drivers
add_subdirectory(drivers)

install(
  FILES README.md
  DESTINATION .)

file(GLOB DEMO_SCRIPTS demo_scripts/*.bat)
install(
  FILES ${DEMO_SCRIPTS}
  DESTINATION .)
