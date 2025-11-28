#
#
#

add_library(mini STATIC
  rtl_output.c
  rtl_output_real.c
  rtl_output_str.c
  rtl_output_bool.c
  rtl_output_nl.c
  rtl_fix.c
  rtl_allocate_array.c
  )

if (MSVC)
    target_compile_definitions(mini PRIVATE -D_NO_CRT_STDIO_INLINE)
endif()

install(TARGETS mini
  ARCHIVE DESTINATION lib
  )
