#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10

void f(int *arg) {
  int delta = *arg;
  int x = 0;
  int limit = N;

  while (-limit < x && x < limit) {
    x += delta;
    printf("(%d): %d %p\n", delta, x, &x);
  }
}

int main() {
  pthread_t t1, t2;
  int d1 = 1, d2 = -1;

  pthread_create(&t1, NULL, (void *) f, &d1);
  pthread_create(&t2, NULL, (void *) f, &d2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}
