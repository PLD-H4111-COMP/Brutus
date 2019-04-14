#include <stdint.h>
#include <stdio.h>

char f(int i)
{
    return i*2+3 > 5;
}

int main(void) {
    int a = 4;
    int b = a = 5;
    int c = b = a = 6;
    int d = !b >= 3 - ~c-- + 'a' * -+-+-+- ++ a;

    if (a && b || c && d) {
        return c;
    } else if (a > b)
    {
        putchar('\n');
    } else {
        putchar('\\');
    }

    putchar('a');
    putchar(b);
    return f(d);
}

