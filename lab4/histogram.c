#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // get b,thread_count,left,right from command line
  int b = atoi(argv[1]);
  int thread_count = atoi(argv[2]);
  double left = atof(argv[3]);
  double right = atof(argv[4]);
  // get n,a1,a2...an from stdin
  int n;
  scanf("%d", &n);
  double *a = (double *)malloc(n * sizeof(double));
  for (int i = 0; i < n; i++) {
    scanf("%lf", &a[i]);
  }
  // allocate histogram
  int *histogram = (int *)malloc(sizeof(int) * b);
  // initialize histogram
  for (int i = 0; i < b; i++) {
    histogram[i] = 0;
  }
  // get range
  double range = right - left;
  // get step
  double step = range / b;
  // set number of threads
  omp_set_num_threads(thread_count);
  // parallel region
  #pragma omp parallel
  {
    // get thread id
    int tid = omp_get_thread_num();
    // get number of elements to process
    int n_elements = n / thread_count;
    // get start and end index
    int start_index = tid * n_elements;
    int end_index = (tid + 1) * n_elements;
    // process elements
    for (int i = start_index; i < end_index; i++) {
      // get element
      double element = a[i];
      // get index
      int index = (int)((element - left) / step);
      // check if index is in range
      if (index >= 0 && index < b) {
        // increment histogram
        #pragma omp atomic
        histogram[index]++;
      }
    }
  }
  // print histogram
  for (int i = 0; i < b; i++) {
    printf("From %.3f to %.3f: %d\n", left + i * step, left + i * step + step,
           histogram[i]);
  }
  return 0;
}