int main() {
  int a = 0;
  int b = 1;
  int dead = 13;
  int c = a + b;
  int d = b - a;
  if (a < d) {
    c -= 2;
    dead = 3;
  } else {
    c += d;
  }
  int e = c + 4;
  return c;
}
