#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id;
} thread_data_t;

int n, num_of_thread, num, method;
double a, b, sum;

sem_t sum_sem;
pthread_mutex_t lock;
int flag;

double f(double x) { return x * x + x; }

void* calculate(void* arg) {
  // trapezoidal integral
  thread_data_t* data = (thread_data_t*)arg;
  double h = (b - a) / n;
  double start = a + data->id * h * num;
  double end = start + h * num;
  double tmp = (f(start) + f(end)) / 2;
  for (int i = 1; i < num; i++) {
    tmp += f(start + i * h);
  }
  tmp *= h;
  if (method == 3) {
    sem_wait(&sum_sem);
    sum += tmp;
    sem_post(&sum_sem);
  } else if (method == 2) {
    pthread_mutex_lock(&lock);
    sum += tmp;
    pthread_mutex_unlock(&lock);
  } else {
    while (data->id != flag)
      ;
    sum += tmp;
    flag = (flag + 1) % num_of_thread;
  }
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  // read a,b,n from stdin
  scanf("%lf%lf%d", &a, &b, &n);

  // read num_of_thread,method from command line
  num_of_thread = atoi(argv[1]);
  method = atoi(argv[2]);

  // init
  if (method == 3)
    sem_init(&sum_sem, 0, 1);
  else if (method == 2)
    pthread_mutex_init(&lock, NULL);

  // create threads
  num = n / num_of_thread;
  pthread_t* threads = (pthread_t*)malloc(num_of_thread * sizeof(pthread_t));
  thread_data_t* data =
      (thread_data_t*)malloc(num_of_thread * sizeof(thread_data_t));
  for (int i = 0; i < num_of_thread; i++) {
    data[i].id = i;
    pthread_create(&threads[i], NULL, calculate, (void*)&data[i]);
  }

  // join threads
  for (int i = 0; i < num_of_thread; i++) {
    pthread_join(threads[i], NULL);
  }

  // print sum
  printf("%f\n", sum);

  // release memory
  free(threads);
  free(data);

  // return
  return 0;
}