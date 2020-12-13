#include <stdio.h>

int main(){
  int a = 1;
  int b = 2;
  if (a < b){
    b = a + b;
  }
  else{
    b = a + b;
  }
  printf("a = %d\nb = %d\n", a, b);
  return 0;
}