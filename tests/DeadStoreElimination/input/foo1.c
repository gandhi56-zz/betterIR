#include <stdio.h>
int main(){
  int a = 0;
  int b = 1;
  int c = 2;
  int d = 4;
  if (a < b){
    a = 4;
    if (a == d){
      a = 123;
      b = 78;
    }
    else{
      a = 78;
      b = 123;
    }
  }
  else{
    c = 333;
  }
  printf("a = %d\nb = %d\nc = %d\nd = %d\n", a, b, c, d);
  return 0;
}