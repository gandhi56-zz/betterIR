
int foo1(int a, int b){
  a = b + 4;
  return a;
}

int main(){
  int a = 1;
  int b = 2;
  int c = 3;
  if (a < b){
    c = foo1(a, b);
  }
  else{
    c = foo1(c, b);
  }
  return 0;
}