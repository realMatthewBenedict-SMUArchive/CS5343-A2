#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE] = {0};

size_t buffer_head = 0;
size_t buffer_tail = 0;

pthread_mutex_t lock;
sem_t* empty = NULL;
sem_t* full = NULL;

#ifdef PRODUCER_DEBUG
size_t empty_guess = BUFFER_SIZE;
size_t full_guess = 0;
#define computed_length ((BUFFER_SIZE + buffer_tail - buffer_head) % BUFFER_SIZE)
#endif

#define inc(var) var = ((var + 1) % BUFFER_SIZE)
#define sleep_rand() sleep(arc4random_uniform(10))

#ifdef __APPLE__
#define sem_open_caller(semaphore, input_name, value) do {\
    char name[128];\
    sprintf(name, "semaphore_%x_%s", (int)getpid(), input_name);\
    *semaphore = sem_open(name, O_CREAT | O_EXCL, O_RDWR, value);\
    if (*semaphore == SEM_FAILED) {\
        printf("Error: Could not init semaphore %s: %s\n", name, strerror(errno));\
        exit(-1);\
    }\
} while(0)
#define sem_init(semaphore, pshared, value) sem_open_caller(&semaphore, #semaphore, value)
#define sem_destroy(semaphore) sem_unlink(#semaphore)
#endif

// Macro to create threads
#define create_threads(threads_array, count, start_routine) pthread_t* threads_array; do {\
    threads_array = malloc(count * sizeof(pthread_t)); \
    for (unsigned long i = 0; i < count; ++i) {\
        pthread_create(&threads_array[i], NULL, start_routine, (void*)i);\
    }\
} while(0)

// Macro to destroy threads
#define destroy_threads(threads_array, count) do {\
    for (unsigned long i = 0; i < count; ++i) {\
        pthread_cancel(threads_array[i]);\
        pthread_join(threads_array[i], NULL);\
    }\
    free(threads_array);\
} while(0)
