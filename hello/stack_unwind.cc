//
//
//

class delete_on_exit {
public:
  char *d;

  delete_on_exit(char *data)
    : d(data)
  {}

  ~delete_on_exit()
  {
    delete []d;
  }
};

extern "C" int foo(int x)
{
  delete_on_exit nothing(0);

  return x * x;
}

extern "C" int foobar(int x)
{
  if (x > 0) {
    delete_on_exit nothing(0);

    return x * x;
  }
  return x * 2;
}
