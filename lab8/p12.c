    #include <stdio.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <stdlib.h>
    #include <unistd.h>
    
    #define BUFFER_SIZE 10 
    
    // Shared buffer
    int buffer[BUFFER_SIZE];
    int in = 0, out = 0;  // Track producer and consumer positions
    
    // Semaphores
    sem_t empty;  // Count of empty slots
    sem_t full;   // Count of full slots
    sem_t mutex;  // Mutex for critical section
    
    // Function prototypes
    void* producer(void* arg);
    void* consumer(void* arg);
    int produce_item();
    void consume_item(int item);
    
    int main() {
        pthread_t prod_thread, cons_thread;
    
        // Initialize semaphores
        sem_init(&empty, 0, BUFFER_SIZE);  // Initially, all slots are empty
        sem_init(&full, 0, 0);             // Initially, no slots are full
        sem_init(&mutex, 0, 1);            // Mutex is initialized to 1
    
        // Create producer and consumer threads
        pthread_create(&prod_thread, NULL, producer, NULL);
        pthread_create(&cons_thread, NULL, consumer, NULL);
    
        // Wait for the threads to finish (in this case, they won't, so it's infinite)
        pthread_join(prod_thread, NULL);
        pthread_join(cons_thread, NULL);
    
        // Destroy the semaphores
        sem_destroy(&empty);
        sem_destroy(&full);
        sem_destroy(&mutex);
    
        return 0;
    }
    
    // Producer function
    void* producer(void* arg) {
        while (1) {
            int item = produce_item();  // Produce an item
    
            sem_wait(&empty);  // Wait for an empty slot
            sem_wait(&mutex);  // Enter critical section
    
            // Place the item in the buffer
            buffer[in] = item;
            printf("Produced item: %d at position %d\n", item, in);
            in = (in + 1) % BUFFER_SIZE;
    
            sem_post(&mutex);  // Exit critical section
            sem_post(&full);   // Signal that the buffer has one more full slot
    
            sleep(1);  // Simulate time taken to produce an item
        }
    }
    
    // Consumer function
    void* consumer(void* arg) {
        while (1) {
            sem_wait(&full);   // Wait for a full slot
            sem_wait(&mutex);  // Enter critical section
    
            // Remove the item from the buffer
            int item = buffer[out];
            printf("Consumed item: %d from position %d\n", item, out);
            out = (out + 1) % BUFFER_SIZE;
    
            sem_post(&mutex);  // Exit critical section
            sem_post(&empty);  // Signal that the buffer has one more empty slot
    
            consume_item(item);  // Consume the item
    
            sleep(2);  // Simulate time taken to consume an item
        }
    }
    
    // Function to simulate item production
    int produce_item() {
        return rand() % 100;  // Produce a random item
    }
    
    // Function to simulate item consumption
    void consume_item(int item) {
        printf("Item %d consumed\n", item);
    }
