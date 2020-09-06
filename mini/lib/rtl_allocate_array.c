//
//
//

#include <stdlib.h>

int *
rtl_allocate_array(int s)
{
  return (int *)calloc(s, 4 /* 32-bit */);
}
