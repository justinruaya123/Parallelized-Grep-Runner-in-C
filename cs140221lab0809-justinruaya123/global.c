#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#define N 100

int global = 0;

void increment() {
  while (global < N) {
    global++;
    printf("%d inc: %d\n", sched_getcpu(), global);
  }
}

void decrement() {
  while (global > -N) { 
    global--;
    printf("%d dec: %d\n", sched_getcpu(), global);
  }
}

int main() {
  pthread_t t1, t2;

  pthread_create(&t1, NULL, (void *) increment, NULL); //creates T1
  pthread_create(&t2, NULL, (void *) decrement, NULL); //creates T2
  //T1 and T2 can switch places everytime.
  pthread_join(t1, NULL); //waits for T1 to finish
  pthread_join(t2, NULL); //waits for T2 to finish

  return 0;
}
