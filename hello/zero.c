/* zero.c */
void puts(char *);
int zero()
{
  return 0;
}

double dzero()
{
  double d = 0.125;

  return 0.125;
}

void tzero(double d)
{
    if(d > 0)
        puts("Greater zero\n");
    else
        puts("Less zero\n");
}
