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
    putchar(++c);
  }
  putchar('\n');
  return pgcd(55, 25);
}

