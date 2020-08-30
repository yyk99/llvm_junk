extern int printf (const char *__restrict __format, ...);

int main()
{
  int i;
  for(i = 1 ; i < 10 ; i = i + i)
    printf("%d\n", i);
}
