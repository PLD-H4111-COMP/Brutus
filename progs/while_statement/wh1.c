int pgcd(int a, int b) {
  int t = 0;
  while(b != 0) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}

int main()
{
  char c = 'A' - 1;
  while(c < 'Z') {
    int i=0;
    ++c;
    while(i<10) {
      putchar(c);
      ++i;
    }
    putchar('\n');
  }
  putchar('\n');
  return pgcd(55, 25);
}

