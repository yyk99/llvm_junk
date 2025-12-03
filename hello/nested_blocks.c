int printf(const char *, ...);
int main()
{
    {
        int r[10];
        printf("%lx\n", (unsigned long)&r);
    }
    {
        int a[5];
        printf("%lx\n", (unsigned long)&a);
    }
}
