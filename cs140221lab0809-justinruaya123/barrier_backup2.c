#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
sem_t sem_pagod_nako[2];
int counter = 3;

void a() {
  while (1) {
    sem_wait(&sem_pagod_nako[0]);
    if(counter == 0) {
      sem_post(&sem_pagod_nako[0]);
      continue;
    }
    counter--;
    fprintf(stderr, "A");
    sem_post(&sem_pagod_nako[0]);
  }
}

void b() {
  while (1) {
    sem_wait(&sem_pagod_nako[0]);
    if(counter > 0) {
      counter--;
      fprintf(stderr, "B");
    }
    sem_post(&sem_pagod_nako[0]);
  }
}

void c() {
  while (1) {
    sem_wait(&sem_pagod_nako[0]);
    if(counter > 0) {
      counter--;
      fprintf(stderr, "C");
    }
    sem_post(&sem_pagod_nako[0]);
  }
}

int main() {
  pthread_t ta, tb, tc;
  sem_init(&sem_pagod_nako[0], 4, 1);
  sem_init(&sem_pagod_nako[0], 4, 1);
  pthread_create(&ta, NULL, (void *) a, NULL);
  pthread_create(&tb, NULL, (void *) b, NULL);
  pthread_create(&tc, NULL, (void *) c, NULL);

  while (1) {
    sem_wait(&sem_pagod_nako[0]);
    if(counter > 0) {
      counter = 3;
      sem_post(&sem_pagod_nako[0]);
      continue;
    }
    fprintf(stderr, " ");
    sem_post(&sem_pagod_nako[0]);
  }
  sem_destroy(&sem_pagod_nako[0]);
  return 0;
}
