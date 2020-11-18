// gcc -std=c99 -fopenmp omp2.c -o omp2
// You fill in
// gcc -std=c99 -fopenmp omp2.c -o omp2
#include <stdio.h>
#include <omp.h>

int main(){

  // Attempt to parallelize this block of code
  #pragma omp parallel
  {
    // Parallel-for loop
    #pragma omp for
    for(int n=0; n <100; ++n){
      printf(" %d",n);
    }
  }
  return 0;
}
