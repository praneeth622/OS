#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5  // Number of philosophers
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];                     // Array to track the state of each philosopher
sem_t mutex;                      // Mutex to protect shared data
sem_t philosophers[N];            // One semaphore per philosopher

void think(int philosopher) {
    printf("Philosopher %d is thinking.\n", philosopher);
    sleep(1);  // Simulate thinking
}

void eat(int philosopher) {
    printf("Philosopher %d is eating.\n", philosopher);
    sleep(2);  // Simulate eating
}

void test(int philosopher) {
    // A philosopher can eat if they are hungry and neither of their neighbors is eating
    if (state[philosopher] == HUNGRY &&
        state[(philosopher + N - 1) % N] != EATING &&
        state[(philosopher + 1) % N] != EATING) {
        
        state[philosopher] = EATING;
        sem_post(&philosophers[philosopher]);  // Allow philosopher to eat
    }
}

void take_forks(int philosopher) {
    sem_wait(&mutex);  // Enter critical section
    state[philosopher] = HUNGRY;
    printf("Philosopher %d is hungry.\n", philosopher);

    // Try to acquire forks to eat
    test(philosopher);

    sem_post(&mutex);  // Exit critical section
    sem_wait(&philosophers[philosopher]);  // Wait for the signal to eat
}

void put_forks(int philosopher) {
    sem_wait(&mutex);  // Enter critical section
    state[philosopher] = THINKING;
    printf("Philosopher %d is putting down forks and starting to think.\n", philosopher);

    // Check if neighboring philosophers can now eat
    test((philosopher + N - 1) % N);
    test((philosopher + 1) % N);

    sem_post(&mutex);  // Exit critical section
}

void* philosopher(void* num) {
    int philosopher = *(int *)num;

    while (1) {
        think(philosopher);
        take_forks(philosopher);
        eat(philosopher);
        put_forks(philosopher);
    }
}

int main() {
    pthread_t thread_id[N];
    int philosopher_ids[N];

    sem_init(&mutex, 0, 1);  // Initialize mutex semaphore
    for (int i = 0; i < N; i++) {
        sem_init(&philosophers[i], 0, 0);  // Initialize each philosopher's semaphore
        state[i] = THINKING;
        philosopher_ids[i] = i;
    }

    // Create philosopher threads
    for (int i = 0; i < N; i++) {
        pthread_create(&thread_id[i], NULL, philosopher, &philosopher_ids[i]);
    }

    // Wait for philosopher threads to complete
    for (int i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL);
    }

    // Cleanup
    sem_destroy(&mutex);
    for (int i = 0; i < N; i++) {
        sem_destroy(&philosophers[i]);
    }

    return 0;
}
