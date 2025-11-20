struct s {
  int p[10];
};
int foo()
{
  struct s ss;

  return ss.p[1];
}
