// gcc -std=c99 -fopenmp omp4.c -o omp4
// You fill in
#include <stdio.h>

int fib_recursive(int n){
  // base case
  if(n < 2){
   return 1;
  }
  // Accumulate our result 
  return fib_recursive(n-1)+fib_recursive(n-2);
}


int main(){
  
  // Computes the 41st number(n+1) in the fibonacci sequence
  printf("%d ",fib_recursive(40));
  printf("\n");
  
  return 0;
}
