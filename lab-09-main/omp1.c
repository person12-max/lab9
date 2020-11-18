// gcc -std=c99 -fopenmp omp1.c -o omp1
// You fill in
// A basic hello world with OMP
// Compile with: gcc -std=c99 -fopenmp omp1.c -o omp1
#include <stdio.h>
#include <omp.h>

int main(){
  
  // Create our pragma, and then within curly braces
  // run OpenMP on this structuerd block.
  #pragma omp parallel
  {
      int ID = omp_get_thread_num();
      printf("hello (%d)\n",ID);
      printf(" world(%d)\n",ID);
  }

  return 0;
}
