int fib(int n) {
  if(n == 0) return 0;
  if(n == 1) return 1;
  return fib(n-1) + fib(n-2);
}

int64_t fact(int64_t n) {
  if(n <= 1) return 1;
  return n * fact(n-1);
}

int64_t lshift(int64_t n, int p) {
  while(--p + 1) n = n * 2;
  return n;
}

int64_t rshift(int64_t n, int p) {
  while(--p + 1) n = n / 2;
  return n;
}

void print(int64_t n) {
  int i;
  for(i=63; i>=0; --i) {
    if(rshift(n, i) & 1 > 0) putchar('1');
    else putchar('0');
  }
}

int main() {
  int i=-1;
  while(++i < 100) {
    print(i);
    putchar('\n');
  }
  return 0;
}
