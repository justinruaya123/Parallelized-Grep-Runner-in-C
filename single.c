#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<limits.h>
#include<assert.h>

// ************** FUNCTION POINTERS **************

// Logic on Queue [CS 140 2223A - Lecture 16]
typedef struct __node_t {
    char file[PATH_MAX];
    struct __node_t *next;
} node_t;
typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    // init mutex head_lock and tail_lock
} queue_t;
// Auxiliary functions
int accessDir(char [], char [], queue_t *, int);
int isFile(char []);
int getGrepMatch(char [], char []);
void work(queue_t *, char [], int);

void Queue_Init(queue_t *);
void Queue_Enqueue(queue_t *, char []);
int Queue_Dequeue(queue_t *, char []);
void Queue_Free(queue_t *);

// ************** CONSTANTS **************
const char slash[] = "/";
// ************** MAIN CODE **************
int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);

    queue_t *QUEUE = malloc(sizeof(queue_t));
    Queue_Init(QUEUE);
    
    char *path = realpath(argv[2], NULL);
    Queue_Enqueue(QUEUE, path);
    work(QUEUE, argv[3], 0);

    free(path);
    Queue_Free(QUEUE);
    return 0;
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


int accessDir(char path[], char word[], queue_t *q, int worker_id) {
    struct dirent *child; //can be a directory or just a file
    char filename[PATH_MAX];
    DIR *dir = opendir(path);
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
    closedir(dir);
    return 0;
}

void work(queue_t *q, char word[], int worker_id) {
    while(q->head != q-> tail) {
        char file[PATH_MAX];
        Queue_Dequeue(q, file);
        accessDir(file, word, q, worker_id);
    }
    
}
 
// ****************************************************************************************************
//                                         QUEUE LOGIC
// ****************************************************************************************************

// CS 140 2223A - Lecture 16
void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    // init head
    // init tail
}

// CS 140 2223A - Lecture 16
void Queue_Enqueue(queue_t *q, char file[]) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp!= NULL);
    strcpy(tmp->file, file);
    tmp->next = NULL;
    // lock tail
    q->tail->next = tmp;
    q->tail = tmp;
    // unlock tail
}

// CS 140 2223A - Lecture 16
int Queue_Dequeue(queue_t *q, char file[]) {
    //lock head
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if(new_head == NULL) {
        //unlock head
        return -1;
    }
    strcpy(file, new_head->file);
    q->head = new_head;
    //unlock head
    free(tmp);
    return 0;
}

// https://stackoverflow.com/questions/6417158/c-how-to-free-nodes-in-the-linked-list
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