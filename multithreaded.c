#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<limits.h>
#include<assert.h>
// Multithreaded stuff
#include<pthread.h>
#include<semaphore.h>

// ************** FUNCTION POINTERS **************

// Logic on Queue [CS 140 2223A - Lecture 16]
typedef struct __node_t {
    char file[PATH_MAX];
    struct __node_t *next;
} node_t;
typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;
} queue_t;
// Auxiliary functions
void accessDir(char [], char [], queue_t *, int);
int isFile(char []);
int getGrepMatch(char [], char []);
void *work(void *);

void Queue_Init(queue_t *);
void Queue_Enqueue(queue_t *, char []);
int Queue_Dequeue(queue_t *, char []);
void Queue_Free(queue_t *);

// ************** CONSTANTS **************
const char slash[] = "/";
pthread_t tid[8];
sem_t sem_work;
int done = 0;
int N = 1;
// ************** THREADING SHENANIGANS **************
int counter = 0;
pthread_mutex_t m_counter = PTHREAD_MUTEX_INITIALIZER;

struct work_args {
    queue_t *q;
    char word[64];
    int worker_id;
} work_args;

struct work_args args[8];


// ****************************************************************************************************
//                                    DRIVER CODE / MAIN
// ****************************************************************************************************
int main(int argc, char *argv[]) {
    N = atoi(argv[1]);
    queue_t *QUEUE = malloc(sizeof(queue_t));
    Queue_Init(QUEUE);
    sem_init(&sem_work, 0, 0);
    
    char *path = realpath(argv[2], NULL);
    
    Queue_Enqueue(QUEUE, path);
    //Create threads and execute code
    for(int x = 0; x < N; x++) {
        args[x].q = QUEUE;
        strcpy(args[x].word, argv[3]);
        args[x].worker_id = x;
        pthread_create(&tid[x], NULL, work, (void *)&args[x]);
    }
    for(int x = 0; x < N; x++) {
        pthread_join(tid[x], NULL);
    }
    sem_destroy(&sem_work);
    free(path);
    Queue_Free(QUEUE);
    return 0;
}

void *work(void *values) {
    struct work_args *args = values;
    while(1) {
        sem_wait(&sem_work);
        if(done) {
            break;
        }
        char file[PATH_MAX];
        Queue_Dequeue(args->q, file);
        accessDir(file, args->word, args->q, args->worker_id);
        pthread_mutex_lock(&args->q->head_lock);
        pthread_mutex_lock(&args->q->tail_lock);
        if(args->q->head == args->q-> tail) {
            pthread_mutex_lock(&m_counter);
            if(counter == 0) {
                done = 1;
                for(int x = 0; x < N; x++) // flush standby threads
                    sem_post(&sem_work);
            }
            pthread_mutex_unlock(&m_counter);
        }
        pthread_mutex_unlock(&args->q->head_lock);
        pthread_mutex_unlock(&args->q->tail_lock);
    }
}
// ****************************************************************************************************
//                                       FILE SYSTEM/GREP FUNCTIONS
// ****************************************************************************************************

// From https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
int isFile(char path[]) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int getGrepMatch(char file_path[], char word[]) {
    char buffer[PATH_MAX]; //Adjust tolerance on grep %s %s
    snprintf(buffer, sizeof(buffer), "grep \"%s\" \"%s\" > /dev/null", word, file_path);
    return system(buffer);
}


void accessDir(char path[], char word[], queue_t *q, int worker_id) {
    struct dirent *child; //can be a directory or just a file
    char filename[PATH_MAX];
    DIR *dir = opendir(path);
    pthread_mutex_lock(&m_counter);
    counter++;
    pthread_mutex_unlock(&m_counter);
    printf("[%d] DIR %s\n", worker_id, path);

    while ((child = readdir(dir)) != NULL) {
        if(!strcmp(child->d_name, ".") || !strcmp(child->d_name, "..")) continue;
        strcpy(filename, path);
        strcat(filename, slash);
        strcat(filename, child->d_name);
        if(isFile(filename)) {
            if(getGrepMatch(filename, word) == 0) {
                printf("[%d] PRESENT %s\n", worker_id, filename);
            } else {
                printf("[%d] ABSENT %s\n", worker_id, filename);
            }
        } else {
            printf("[%d] ENQUEUE %s\n", worker_id, filename);
            Queue_Enqueue(q, filename);
        }
    }
    pthread_mutex_lock(&m_counter);
    counter--;
    pthread_mutex_unlock(&m_counter);
    closedir(dir);
}
 
// ****************************************************************************************************
//                                         QUEUE LOGIC
// ****************************************************************************************************

// CS 140 2223A - Lecture 16
void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

// CS 140 2223A - Lecture 16
void Queue_Enqueue(queue_t *q, char file[]) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp!= NULL);
    strcpy(tmp->file, file);
    tmp->next = NULL;
    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
    sem_post(&sem_work);
}

// CS 140 2223A - Lecture 16
int Queue_Dequeue(queue_t *q, char file[]) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if(new_head == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1;
    }
    strcpy(file, new_head->file);
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}

// https://stackoverflow.com/questions/6417158/c-how-to-free-nodes-in-the-linked-list
// To be executed ONLY on the main tread, after the threads are waiting.
void Queue_Free(queue_t *q) {
    node_t *tmp = q->head;
    while(q->head != q->tail) {
        tmp = q->head;
        q->head = q->head->next;
        free(tmp);
    }
    free(q->tail);
    free(q);
}