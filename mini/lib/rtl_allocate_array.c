//
//
//

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int *
rtl_allocate_array(int s, int n)
{
    fprintf(stderr, "rtl_allocate_array(%d, %d)\n", s, n);
    assert(s > 0 && n > 0);
  
    int *vec = (int *)calloc(s, n);
#if !NDEBUG
    for(int i = 0 ; i != (s*n)/sizeof(int) ; ++i) {
        vec[i] = i+1;
    }
#endif
    return vec;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
