# Copyright: 2011-2012 ARM Ltd
# $Id$
# $URL$

# default version is 2
set(RDDI_API_VERSION 2)

# macro to produce the correct library name for this platform
#  LIBVAR is the variable to set
#  NAME is the base name of the library (e.g. rddi-debug-foo)
macro(rddi_library_name LIBVAR NAME)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(libSuffix "_${RDDI_API_VERSION}")
  endif()
  set(${LIBVAR} "${NAME}${libSuffix}")
endmacro()

# macro to add platform specific properties to an RDDI build
#  LIBNAME is the name of the library (e.g. rddi-debug-foo)
macro(rddi_library_properties LIBNAME)
  if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    # Ensure we get .so.2 suffix
    set_target_properties(${LIBNAME} PROPERTIES SOVERSION ${RDDI_API_VERSION})
  
    # set linker flags
    #   ensure RPATH points to same dir as lib
    set(linkFlags "-Wl,-rpath,$ORIGIN")
    #   ensure no undefined symbols
    #   TODO: Mac OS equivalent
    if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
      set(linkFlags "${linkFlags} -Wl,-z,defs")
    endif()
    set_target_properties(${LIBNAME} PROPERTIES LINK_FLAGS "${linkFlags}")

    # Remove build directories from RPATH
    set_target_properties(${LIBNAME} PROPERTIES SKIP_BUILD_RPATH true)
  endif()
endmacro()
