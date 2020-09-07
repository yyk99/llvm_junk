//
//
//

#include <stdlib.h>
#include <assert.h>

int *
rtl_allocate_array(int s)
{
  assert(s > 0);
  
  int *vec = (int *)calloc(s, 4 /* 32-bit */);
#if !NDEBUG
  for(int i = 0 ; i != s ; ++i) {
	vec[i] = i+1;
  }
#endif
  return vec;
}
