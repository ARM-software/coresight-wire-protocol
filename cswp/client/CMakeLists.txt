# Build static library for client
add_library(cswp_client
  STATIC
  cswp_client_commands.c
  cswp_client.c
  )
set_property(TARGET cswp_client PROPERTY POSITION_INDEPENDENT_CODE ON)
