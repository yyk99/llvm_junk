struct pair_t {
  int first;
  int second;
};

int main()
{
  struct pair_t ss;

  ss.first = 1;
  ss.second = 2;

  return ss.first + ss.second;
}
