void printNumber(int a)
{
    if (!a)
    {
        putchar('0');
    }
    else
    {
        int leading_zero = 1;
        int i;
        for (i = 1000000000; i > 0; i = i/10)
        {
            int d = a/i%10;
            if (d)
            {
                leading_zero = 0;
                putchar('0'+d);
            }
            else if (!leading_zero)
                putchar('0');
        }
    }
}

int fib(int n)
{
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib(n-1)+fib(n-2);
}

int main(void)
{
    int i;
    for (i = 0; i < 50; ++i)
    {
        printNumber(i);
        putchar(':');
        putchar(' ');
        printNumber(fib(i));
        putchar('\n');
    }
    return 0;
}
