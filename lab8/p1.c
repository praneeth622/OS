#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem_B2;
sem_t sem_A2;

void* thread_A(void* arg) {
    
    printf("A1\n");
    
    sem_post(&sem_A2);  

    sem_wait(&sem_B2);  
    printf("A2\n");

    return NULL;
}

// Function for Thread B
void* thread_B(void* arg) {
    
    printf("B1\n");
    
    sem_post(&sem_B2);
    sem_wait(&sem_A2);  
    printf("B2\n");

    return NULL;
}

int main() {
    pthread_t tid_A, tid_B;

    sem_init(&sem_B2, 0, 0);
    sem_init(&sem_A2, 0, 0);

    pthread_create(&tid_A, NULL, thread_A, NULL);
    pthread_create(&tid_B, NULL, thread_B, NULL);

    pthread_join(tid_A, NULL);
    pthread_join(tid_B, NULL);

    sem_destroy(&sem_B2);
    sem_destroy(&sem_A2);

    return 0;
}
