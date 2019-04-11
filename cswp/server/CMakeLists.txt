
add_library(cswp_server
  STATIC
  cswp_server_commands.c
  cswp_server_cmdint.c
  cswp_server_impl.c
  )
set_property(TARGET cswp_server PROPERTY POSITION_INDEPENDENT_CODE ON)
