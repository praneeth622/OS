#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Global variables and semaphores
sem_t mutex, wrt; // Semaphores to control access
int readCount = 0; // Number of readers reading

// Reader function
void *reader(void *readerID) {
    int id = *(int *)readerID;

    // Entry section
    sem_wait(&mutex); // Reader gets access to change readCount
    readCount++; // Increment the number of readers
    if (readCount == 1) {
        sem_wait(&wrt); // If it's the first reader, block the writer
    }
    sem_post(&mutex); // Allow other readers to increment readCount

    // Critical section (Reading)
    printf("Reader %d is reading\n", id);
    sleep(1); // Simulate reading time

    // Exit section
    sem_wait(&mutex); // Reader gets access to change readCount
    readCount--; // Decrement the number of readers
    if (readCount == 0) {
        sem_post(&wrt); // If no more readers, allow the writer
    }
    sem_post(&mutex); // Allow other readers to decrement readCount

    return NULL;
}

// Writer function
void *writer(void *writerID) {
    int id = *(int *)writerID;

    // Entry section
    sem_wait(&wrt); // Writer gets exclusive access to write

    // Critical section (Writing)
    printf("Writer %d is writing\n", id);
    sleep(1); // Simulate writing time

    // Exit section
    sem_post(&wrt); // Allow other readers or writers

    return NULL;
}

int main() {
    pthread_t readers[5], writerThread;
    int readerIDs[5], writerID = 1;

    // Initialize semaphores
    sem_init(&mutex, 0, 1); // mutex initialized to 1
    sem_init(&wrt, 0, 1); // wrt initialized to 1

    // Create one writer thread
    pthread_create(&writerThread, NULL, writer, &writerID);

    // Create five reader threads
    for (int i = 0; i < 5; i++) {
        readerIDs[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &readerIDs[i]);
    }

    // Wait for writer thread to finish
    pthread_join(writerThread, NULL);

    // Wait for all reader threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
