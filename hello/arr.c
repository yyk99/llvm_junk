//
//
//

int arr[10];

int foo(int k)
{
    return arr[k];
}

int bar(int k)
{
    int arr[10]; // = {1,2,3,4,5,6,7,8,9,0};

    return arr[k];
}

int foobar (int k)
{
    struct {int arr[10]; } s;

    return s.arr[k];
}

struct ss { int arr[10]; };

int foodog(int k,  struct ss s)
{
    return s.arr[k];
}

int baz()
{
    int k = 3;
    struct ss s;
    return foodog(k, s);
}

struct ss biz()
{
    struct ss r;
    r.arr[0] = 99;
    r.arr[1] = 98;

    return r;
}

int biz_call(int k)
{
    return biz().arr[k];
}
