# Lab 9 Parallel Programming

We  discovered that if we want to write parallel software, our compilers are not yet capable of figuring out our intentions and automatically parallelizing our code. However, we   explored how a tool like [OpenMP](https://www.openmp.org/) can be used to provide hints and try to make code run in parallel by using threads. In today's lab, we are going to work with the OpenMP library and its #pragma statements.

# Motivation

Writing code that can run in parallel is important for reasons of [performance](http://www.gotw.ca/publications/concurrency-ddj.htm) and [reducing power consumption](https://en.wikipedia.org/wiki/Dennard_scaling). In languages like C (C++, Rust, etc.) parallel programming extensions like OMP being used in industry today.  We  explored a variant of this idea in our artists  lab.

## Part 1 - Finding your machines specs

* Type `lscpu` on the terminal to find out how many cpus you have.
* You can additionally find some information from the kernel by:
  * `cd /sys/devices/system/cpu/cpu0/cache/`
  * then `cd index0` and run `cat coherency_line_size`
    * This gives us a report of how big the cacheline size is.
  * Progamattically, you may be able to query `sysconf (_SC_LEVEL1_DCACHE_LINESIZE)` for example to find your cache line size.
    * [sysconf man page](http://man7.org/linux/man-pages/man3/sysconf.3.html)
    * [Stack overflow article](https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size)

Note, if you are curious if you have GPUs, on Linux you can try on the terminal: [`lspci`](https://linux.die.net/man/8/lspci) (or look under Systems/Preferences/Hardware Information.) Note, there may or may not be GPUs available.

## Part 2 - OpenMP

Today, we are going to  work on a fork-join parallelism task using OpenMP.

### Task 1 - OpenMP

#### Example 1 - Hello World

Let's first do a little hello world to make sure OpenMP is setup and running properly. The thing to note is that we need to include an OpenMP compiler flag to make sure the library is properly linked in.

Try writing in the following example in [omp1.c](./omp1.c)

```c
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
```

- Compile with: `gcc -std=c99 -fopenmp omp1.c -o omp1`
- Run the program with `./omp1`.

**Answer in a sentence here:** *what your output is, and whether the expected output is what you would expect.* 
**If you can discuss this with your current teammate or another classmate, please do.**

I am getting hello(i), world(i) printed out a number of times where i is the id : This is expected because we are printing from multiple threads that have different ids, and the smallest id is 0 and the largest is 48. 

#### Example 2 - For-loops

Loops are a common structure that can be explored for potential parallelism.  However, in order for it to be successful, we need to know the exact number of iterations.

Try writing in the following example in [omp2.c](./omp2.c)

```c
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
```

- Compile with: `gcc -std=c99 -fopenmp omp2.c -o omp2`
- Run the program with `./omp2`.


**Answer:** *Why is our output not ordered?* 
**If you can discuss this  with your current teammate or another classmate, please do.**

It is not ordered because we are using multiple threads and we don’t know what order the threads run in the for loop. 

#### Example 3 - Synchronization

The previous example does a nice job of spawning many threads that print off the thread id. However, we do not have any information about how many threads were launched within our parallel-for loop. Let's go ahead and record this information, by storing the number of threads launched. In addition, we will also add some synchronization (i.e. using the barrier pattern) to print the number of threads. The barrier pattern waits for all active threads to arrive, and then proceeds.

Try writing in the following example in [omp3.c](./omp3.c)

```c
// gcc -std=c99 -fopenmp omp3.c -o omp3
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

```

- Compile with: `gcc -std=c99 -fopenmp omp3.c -o omp3`
- Run the program with `./omp3`.

#### Example 4 - Fibonacci Sequence

For our first example, take a look at the following code which generates a number in the [fibonacci sequence](https://en.wikipedia.org/wiki/Fibonacci_number).

Try writing in the following example in [omp4.c](./omp4.c)

```c
// This program computes the fibonacci sequence.
// Compile with: gcc -std=c99 -fopenmp omp4.c -o omp4

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
```

- Compile with: `gcc -std=c99 -fopenmp omp4.c -o omp4`
**Run the program above**: With `time ./omp4`

So the question now is, with this algorithm is there any potential parallelism? Well, in our current implementation, not so much. We need to transform our algorithm slightly, so we can remove data dependencies.

Take a look below and see how OpenMP can be used to separate work into different tasks. Update [omp4.c](./omp4.c) with the below snippet.

```c
// A new snippet
// Compile with: gcc -std=c11 -fopenmp omp4.c -o omp4

int fib_recursive(int n){
  // base case
  if(n < 2){
   return 1;
  }
  // Some shared variables
  int x,y;

  // Accumulate our result
  #pragma omp task shared(x) 
  x = fib_recursive(n-1);
  #pragma omp task shared(y)
  y = fib_recursive(n-2);
  #pragma omp taskwait
  return x + y;
}
```

**Run the program above**: With `time ./omp4`

Note that you may not see any performance gain--why?

**Answer in a sentence here:** *why there may not be any performance gain here*.  
**If you can discuss this with your current teammate or another classmate, please do.**

Because there is a lot of overhead with task creation and management that is Larter than the work being done. A possible solution to fix this could be to switch between serial and parallels implementations : Define to use parallelism over a certain threshold and use serial version if n is below that threshold. 

#### Example 5 - Computing PI 3.1415....

You may remember that we watched an OpenMP [tutorial video](https://www.youtube.com/watch?v=FQ1k_YpyG_A&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG&index=6) on computing PI. Let's go ahead and type in this code, first in serial, and then you can parallelize it. Again, this is a nice example, because you can check that your solution does indeed add up to PI.

Try writing in the following example in [omp5.c](./omp5.c)

```c
// gcc -std=c11 -fopenmp omp5.c -o omp5
#include <stdio.h>

static long num_steps = 100000000;
double step;

int main(){
  int i;
  double x, pi, sum = 0.0;
  
  step = 1.0/(double)num_steps;
  for(i =0; i < num_steps; i++){
    x = (i+0.5)*step;
    sum = sum+ 4.0/(1.0+x*x);
  }
  pi = step * sum;
  
  printf("PI = %f\n", pi);
 
  return 0;
}

```

Now we are going to try to solve this problem in parallel.

In general, this is a type of [Reduction](http://www.drdobbs.com/architecture-and-design/parallel-pattern-7-reduce/222000718) algorithm, in that we are chunking our problem and solving pieces of it, and then rebuilding the result.

**Discuss with your current teammate or another classmate and answer:** *What is a strategy for solving this problem? Try to avoid false sharing by adding the amount of padded bits needed programmatically*

Now implement in [omp5.c](./omp5.c) your strategy and time it: Implement your strategy and fill out the table below with how much time it takes to complete given the number of threads.  Use [omp_get_wtime()](https://www.openmp.org/spec-html/5.0/openmpsu160.html#x199-9660003.4.1).

Note: You can revisit [Tim Mattson's video](https://www.youtube.com/watch?v=OuzYICZUthM&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG&index=7) if you get stuck for one potential solution.

Note: If there is a lot of activity on the server you may not see as fast of a speedup with multiple threads. Also note that the real time (wall clock time) and user time (time allocated to cpus, or perhaps aggregated from multiple cpus) may differ quite a bit!

<table>
  <tbody>
    <tr>
      <th>1 thread(ms)</th>
      <th>2 threads(ms)</th>
      <th>8 threads(ms)</th>          
    </tr>
    <tr>
      <td>0m1.397s(Real)</td>
      <td>0m0.725s(Real)</td>
      <td>0m0.208s(Real)</td>
    </tr>   
  </tbody>
</table>

#### Example 6 - Quick Sort (+0.5 bonus)

Do a little bit of research to understand [quick sort](https://en.wikipedia.org/wiki/Quicksort). Implement your own parallel version of quick sort in [omp6.c](./omp6.c) to sort 10000-100000 randomly generated numbers. You may NOT use any sorting functions, e.g., qsort().  A nice sample pdf of [Quick Sort](https://www.openmp.org/wp-content/uploads/sc16-openmp-booth-tasking-ruud.pdf) is provided if you get stuck.

![alt text](https://upload.wikimedia.org/wikipedia/commons/6/6a/Sorting_quicksort_anim.gif "Quicksort from wikipedia animation")


## Lab Deliverable

**1. Complete Part 2, task 1 and commit your 5 OpenMP examples to the repository.**
   * Modify and implement the parallel solutions in: omp1.c omp2.c omp3.c omp4.c omp5.c
   * Fill out the table in Example 5.

**2. (Optional) For a 0.5 point bonus on this lab, try to implement quicksort.** 
   * Your code must be named myqsort.c.  
   * myqsort.c accepts a single input: the name of a file containing a single column of integers.  You will need to determine the number of integers in the file.  
   * myqsort.c must write to STDOUT only the sorted integers, one integer per line, starting with the minimum value.
   * As this is an optional EC task, there will be minimal TA support to complete it.

## More Resources to Help

* The following guide provides some insight into using OpenMP. 
  * [OpenMP 4.0 API C/C++ Syntax Quick Reference Card](https://www.openmp.org/wp-content/uploads/OpenMP-4.0-C.pdf)
  * https://bisqwit.iki.fi/story/howto/openmp/#ParallelConstruct 
  * Lawrence Livermore National Labroratory https://computing.llnl.gov/tutorials/openMP/exercise.html 
  * Tim Mattson's video series https://www.youtube.com/watch?v=nE-xN4Bf8XI&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG&index=1

* The following resources may be useful to observe for those interested in GPGPU programming
   * OpenCL for MacOS https://developer.apple.com/opencl/
   * The following guide will get you started for CUDA: https://devblogs.nvidia.com/even-easier-introduction-cuda/
   * The following guide will get you started for OpenCL: https://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/
   
* For fun
  * Parallel Programming Patterns: http://www.drdobbs.com/architecture-and-design/parallel-pattern-7-reduce/222000718
  * See other Parallel Programming libraries in C/C++ here: https://www.youtube.com/watch?v=y0GSc5fKtl8

## Going Further

- Investigate OpenMP's `#pragma omp simd`
