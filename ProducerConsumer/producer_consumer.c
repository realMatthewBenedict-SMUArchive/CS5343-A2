#include "producer_consumer.h"

int insert_item(buffer_item item) {
    // Acquire locks
    sem_wait(empty);
    pthread_mutex_lock(&lock);

    // Debug output
#ifdef PRODUCER_DEBUG
    printf("Producer insert! Head = %zu, tail = %zu (computed length = %zu) ", buffer_head, buffer_tail, computed_length);
    --empty_guess;
    ++full_guess;
    printf("%zu, %zu ", empty_guess, full_guess);
#endif

    // Critical section
    buffer[buffer_tail] = item;
    inc(buffer_tail);

    // Release locks
    sem_post(full);
    pthread_mutex_unlock(&lock);
    return 0;
}

int remove_item(buffer_item* item) {
    // Acquire locks
    sem_wait(full);
    pthread_mutex_lock(&lock);

    // Debug output
#ifdef PRODUCER_DEBUG
    printf("Consumer remove! Head = %zu, tail = %zu (computed length = %zu) ", buffer_head, buffer_tail, computed_length);
    --full_guess;
    ++empty_guess;
    printf("%zu, %zu ", empty_guess, full_guess);
#endif

    // Critical section
    *item = buffer[buffer_head];
    inc(buffer_head);

    // Release locks
    sem_post(empty);
    pthread_mutex_unlock(&lock);
    return 0;
}

void* producer(void* param) {
    buffer_item item;

    while (1) {
        sleep_rand();
        item = arc4random();
        if (insert_item(item))
            printf("Producer failed to insert\n");
        else
            printf("Producer produced %d\n", item);
    }
}

void* consumer(void* param) {
    buffer_item item;

    while (1) {
        sleep_rand();
        if (remove_item(&item))
            printf("Consumer failed to remove\n");
        else
            printf("Consumer consumed %d\n", item);
    }
}

int main(int argc, char** argv) {
    // Get command line arguments
    if (argc != 4) {
        printf("Usage: %s sleeptime producers consumers", argv[0]);
        return -1;
    }

    unsigned long sleeptime = strtoul(argv[1], NULL, 10);
    unsigned long producers = strtoul(argv[2], NULL, 10);
    unsigned long consumers = strtoul(argv[3], NULL, 10);

    // Initialize semaphores used for buffer
    pthread_mutex_init(&lock, NULL);
    sem_init(full, 0, 0);
    sem_init(empty, 0, BUFFER_SIZE);

    // Create producer threads
    create_threads(producer_threads, producers, &producer);
    // Create consumer threads
    create_threads(consumer_threads, consumers, &consumer);
    // Sleep
    sleep(sleeptime);

    // Destroy threads
    destroy_threads(producer_threads, producers);
    destroy_threads(consumer_threads, consumers);
    
    // Remove semaphores
    pthread_mutex_destroy(&lock);
    sem_destroy(full);
    sem_destroy(empty);
    return 0;
}
