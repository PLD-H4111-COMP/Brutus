int main()
{
  int k=0;
  int i;
  for(i=0; i<10; ++i) {
    int j;
    for(j=0; j<10; ++j) {
      ++k;
    }
  }
  return k;
}
