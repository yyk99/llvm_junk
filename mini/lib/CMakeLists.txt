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
  rtl_exit.c
  )

if (MSVC)
    target_compile_definitions(mini PRIVATE -D_NO_CRT_STDIO_INLINE)
    target_compile_options(mini PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif()

install(TARGETS mini
  ARCHIVE DESTINATION lib
  )
