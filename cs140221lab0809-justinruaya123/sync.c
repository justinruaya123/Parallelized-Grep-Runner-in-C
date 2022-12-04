#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4
#define EACH 1000000

sem_t mutex;
uint64_t sum = 0;

void add() {
  for (int i=0; i < EACH; i++) {
    sem_wait(&mutex);
    sum += 1;
    sem_post(&mutex);
  }
}

int main() {
  pthread_t thread[NTHREADS];
  sem_init(&mutex, 0, 1);
  for (int i = 0; i < NTHREADS; i++) {
    pthread_create(&thread[i], NULL, (void *) add, NULL);
  }

  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  printf("Sum: %lu\n", sum);
  sem_destroy(&mutex);
  return 0;
}
