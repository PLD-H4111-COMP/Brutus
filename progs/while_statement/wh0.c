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
  int c = 'a' - 1;
  while(c != 'z') {
    putchar(++c);
  }
  return pgcd(55, 25);
}

