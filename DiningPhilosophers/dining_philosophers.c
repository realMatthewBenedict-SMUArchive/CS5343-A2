#include "dining_philosophers.h"

void pickup_chopsticks(int num) {
    pthread_mutex_lock(&lock);
    bool sufficient_chopsticks = condition(num);
    if (sufficient_chopsticks) {
        buffer[num] = EATING;
        pthread_mutex_unlock(&lock);
        printf("Philosopher %d picked up chopsticks\n", num);
    } else {
        printf("Philosopher %d is sad because there are not enough chopsticks left\n", num);
        pthread_cond_wait(&sad_philosophers[num], &lock);
    }
}

void return_chopsticks(int num) {
    pthread_mutex_lock(&lock);
    buffer[num] = THINKING;
    printf("Philosopher %d put down chopsticks\n", num);
    if (condition(left(num))) {
        printf("Philosopher %d received chopstick from %d\n", left(num), num);
        buffer[left(num)] = EATING;
        pthread_cond_signal(&sad_philosophers[left(num)]);
    }
    if (condition(right(num))) {
        printf("Philosopher %d received chopstick from %d\n", right(num), num);
        buffer[right(num)] = EATING;
        pthread_cond_signal(&sad_philosophers[right(num)]);
    }
    pthread_mutex_unlock(&lock);
}

void* philosopher(void* param) {
    size_t num = (size_t)param;
    printf("Philosopher %d has arrived\n", (int)num);
    while (1) {
        // Think
        printf("Philosopher %d is thinking deeply\n", (int)num);
        sleep_rand();
        
        // Try to eat
        pthread_mutex_lock(&lock);
        buffer[num] = HUNGRY;
        pthread_mutex_unlock(&lock);
        printf("Philosopher %d is hungry\n", (int)num);
        pickup_chopsticks(num);

        // Eat
        sleep_rand();
        return_chopsticks(num);
    }
}

int main(int argc, char** argv) {
    // Get command line arguments
    if (argc != 2) {
        printf("Usage: %s sleeptime", argv[0]);
        return -1;
    }
    unsigned long sleeptime = strtoul(argv[1], NULL, 10);

    // Initialize locks used for buffer
    pthread_mutex_init(&lock, NULL);
    for (unsigned long i = 0; i < BUFFER_SIZE; ++i) {
        pthread_cond_init(&sad_philosophers[i], NULL);
    }

    // Create philosopher threads
    create_threads(philosopher_threads, BUFFER_SIZE, &philosopher);
    // Sleep
    sleep(sleeptime);

    // Destroy philosopher threads
    destroy_threads(philosopher_threads, BUFFER_SIZE);

    // Destroy locks
    pthread_mutex_destroy(&lock);
    for (unsigned long i = 0; i < BUFFER_SIZE; ++i) {
        pthread_cond_destroy(&sad_philosophers[i]);
    }
    
    return 0;
}
