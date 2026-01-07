#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    THINKING = 0,
    HUNGRY,
    EATING
} buffer_item;
#define BUFFER_SIZE 5

// Initialize buffer to thinking state
buffer_item buffer[BUFFER_SIZE] = {0};

// Mutex and condition variables used
pthread_mutex_t lock;
pthread_cond_t sad_philosophers[BUFFER_SIZE];

// Debug stuff to ensure it's working correctly, disabled
#undef PHILOSOPHER_DEBUG
#ifdef PHILOSOPHER_DEBUG
size_t empty_guess = BUFFER_SIZE;
size_t full_guess = 0;
#define computed_length ((BUFFER_SIZE + buffer_tail - buffer_head) % BUFFER_SIZE)
#endif

// Macros to find neighbor philosophers
#define left(philosopher) ((philosopher + BUFFER_SIZE - 1) % BUFFER_SIZE)
#define right(philosopher) ((philosopher + 1) % BUFFER_SIZE)
// Macro to check if a philosopher can eat and wants to
#define condition(philosopher) (buffer[left(philosopher)] != EATING && buffer[right(philosopher)] != EATING && buffer[philosopher] == HUNGRY)
// Macro to sleep for a random amount of time from 0 to 10 seconds
#define sleep_rand() sleep(arc4random_uniform(10))

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
