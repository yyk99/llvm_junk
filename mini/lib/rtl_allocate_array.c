//
//
//

#include <stdlib.h>
#include <assert.h>

int *
rtl_allocate_array(int s, int n)
{
  assert(s > 0);
  
  int *vec = (int *)calloc(s, n);
#if !NDEBUG
  for(int i = 0 ; i != (s*n)/sizeof(int) ; ++i) {
	vec[i] = i+1;
  }
#endif
  return vec;
}
