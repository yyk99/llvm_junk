int a[3];

int foo(int k)
{
  a[0] = k;
  a[1] = k;
  a[2] = k;

  return 0;
}

int bar(int k)
{
  a[k] = 20;

  return 0;
}
