#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 3

void *thread_func(void *arg) {
    int thread_num = *(int *)arg;
    struct sched_param param;
    int policy;

    pthread_getschedparam(pthread_self(), &policy, &param);

    // Print thread details
    printf("Thread %d started with policy: %s, priority: %d\n", thread_num,
           (policy == SCHED_FIFO) ? "SCHED_FIFO" :
           (policy == SCHED_RR) ? "SCHED_RR" : "SCHED_OTHER",
           param.sched_priority);

    // Simulate work
    for (int i = 0; i < 5; i++) {
        printf("Thread %d running (iteration %d)...\n", thread_num, i + 1);
        sleep(1);  // Simulate some processing time
    }

    printf("Thread %d finished\n", thread_num);
    pthread_exit(NULL);
}

void create_thread_with_attributes(int thread_num, int policy, int priority) {
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;

    // Initialize thread attributes
    pthread_attr_init(&attr);

    // Set scheduling policy and priority
    pthread_attr_setschedpolicy(&attr, policy);
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);

    // Create thread
    if (pthread_create(&thread, &attr, thread_func, &thread_num) != 0) {
        perror("Failed to create thread");
        exit(1);
    }

    pthread_attr_destroy(&attr);  // Clean up
    pthread_join(thread, NULL);   // Wait for thread to finish
}

int main() {
    printf("Starting thread scheduling demonstration...\n");

    // Check if the system allows setting priority
    struct sched_param param;
    int policy = SCHED_FIFO;
    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        printf("Current scheduling policy: %s\n",
               (policy == SCHED_FIFO) ? "SCHED_FIFO" :
               (policy == SCHED_RR) ? "SCHED_RR" : "SCHED_OTHER");
    } else {
        perror("Failed to get scheduling policy");
    }

    printf("\nScenario 1: SCHED_OTHER (default) with default priority\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        create_thread_with_attributes(i, SCHED_OTHER, 0);
    }

    printf("\nScenario 2: SCHED_FIFO with different priorities\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        create_thread_with_attributes(i, SCHED_FIFO, 20 + i);
    }

    printf("\nScenario 3: SCHED_RR with same priority\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        create_thread_with_attributes(i, SCHED_RR, 30);
    }

    printf("Thread scheduling demonstration completed.\n");
    return 0;
}
