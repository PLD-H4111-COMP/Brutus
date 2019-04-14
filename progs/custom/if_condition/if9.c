int foo(int p)
{
  if(p < 2) {
    return 10;
  } else if(p == 5) {
    return 20;
  } else {
    return 0;
  }
}

int main()
{
  int a = 7;
  int b = 0;
  if(foo(1) == 10) {
    if(1+8 == 6-0) {
      b = 3;
    } else {
      b = 9;
    }
  } else {
    b = 17;
    if(1+7 == 6-0) {
      b = 7;
    } else {
      b = 8;
    }
  }
  return b;
}
