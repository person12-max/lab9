// gcc -std=c99 -fopenmp omp3.c -o omp3
// You fill in
#include <stdio.h>
#include <omp.h>

int main(){
   int nthreads;

  // Attempt to parallelize this block of code
  #pragma omp parallel
  {
    // Parallel-for loop
    #pragma omp for
    for(int n=0; n <100; ++n){
      printf(" %d",n);
    }
    // Master thread waits
    #pragma omp barrier
    if(omp_get_thread_num()==0){
        nthreads = omp_get_num_threads();
    }
  }

  printf("\nSolved problem with %d threads\n",nthreads);
 
  return 0;
}
