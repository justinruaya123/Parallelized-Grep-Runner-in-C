#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define NUMS 100000000
#define NTHREADS 15
#define NUMS_PER_THREAD (NUMS / NTHREADS)

struct thread_data {
  uint64_t *arr;
  int start;
  uint64_t ret;
};

void add(struct thread_data *data) {
  int stop = MIN(data->start + NUMS_PER_THREAD, NUMS);
  uint64_t total = 0;

  for (int i = data->start; i < stop; i++) {
    total += data->arr[i];
  }

  data->ret = total;
}

int main() {
  pthread_t thread[NTHREADS];
  struct thread_data data[NTHREADS];

  uint64_t *arr = malloc(NUMS * sizeof(uint64_t));
  for (int i = 0; i < NUMS; i++) {
    arr[i] = i + 1;
  }

  for (int i = 0; i < NTHREADS; i++) {
    data[i].arr = arr;
    data[i].start = i * NUMS_PER_THREAD;
    pthread_create(&thread[i], NULL, (void *) add, &data[i]);
  }

  uint64_t sum = 0;

  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(thread[i], NULL);
    sum += data[i].ret;
  }

  printf("Sum: %lu\n", sum);
  free(arr);

  return 0;
}
