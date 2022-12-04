#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int atomizer() {
    printf("69420");
}

int main() {
  pthread_t t1;

  pthread_create(&t1, NULL, (void *) atomizer, NULL); //creates T1
  pthread_join(t1, NULL); //waits for T1 to finish

  return 0;
}
