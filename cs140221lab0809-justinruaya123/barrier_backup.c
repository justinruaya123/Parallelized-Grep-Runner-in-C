#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void a() {
  while (1) {
    fprintf(stderr, "A");
  }
}

void b() {
  while (1) {
    fprintf(stderr, "B");
  }
}

void c() {
  while (1) {
    fprintf(stderr, "C");
  }
}

int main() {
  pthread_t ta, tb, tc;

  pthread_create(&ta, NULL, (void *) a, NULL);
  pthread_create(&tb, NULL, (void *) b, NULL);
  pthread_create(&tc, NULL, (void *) c, NULL);

  while (1) {
    fprintf(stderr, " ");
  }

  return 0;
}
