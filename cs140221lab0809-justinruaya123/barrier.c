#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem_pagod_nako[4];

void a() {
  while (1) {
    sem_wait(&sem_pagod_nako[0]);
    fprintf(stderr, "A");
    sem_post(&sem_pagod_nako[1]);
  }
}

void b() {
  while (1) {
    sem_wait(&sem_pagod_nako[1]);
    fprintf(stderr, "B");
    sem_post(&sem_pagod_nako[2]);
  }
}

void c() {
  while (1) {
    sem_wait(&sem_pagod_nako[2]);
    fprintf(stderr, "C");
    sem_post(&sem_pagod_nako[3]);
  }
}

int main() {
  pthread_t ta, tb, tc;
  sem_init(&sem_pagod_nako[0], 0, 1);
  sem_init(&sem_pagod_nako[1], 0, 0);
  sem_init(&sem_pagod_nako[2], 0, 0);
  sem_init(&sem_pagod_nako[3], 0, 0);
  
  pthread_create(&ta, NULL, (void *) a, NULL);
  pthread_create(&tb, NULL, (void *) b, NULL);
  pthread_create(&tc, NULL, (void *) c, NULL);

  while (1) {
    sem_wait(&sem_pagod_nako[3]);
    fprintf(stderr, " ");
    sem_post(&sem_pagod_nako[0]);
  }
  sem_destroy(&sem_pagod_nako[0]);
  sem_destroy(&sem_pagod_nako[1]);
  sem_destroy(&sem_pagod_nako[2]);
  sem_destroy(&sem_pagod_nako[3]);
  return 0;
}
