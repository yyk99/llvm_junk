struct s_t {
  int p[10];
} S;

int foo()
{
  struct s_t s;

  return s.p[1];
}

int foobar (int i)
{
  return S.p[i];
}
