#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_USERS 5           // Number of users
#define TIME_FRAME 1000       // Total time to run simulation
#define COLLISION_PROB 0.5    // Probability of collision
#define MAX_DELAY 3           // Maximum delay for retransmission

typedef struct {
    int id;
    int has_data;
    int delay;
} User;

void transmit(User *user) {
    if (user->has_data) {
        printf("User %d is attempting to transmit data...\n", user->id);
        if ((float) rand() / RAND_MAX < COLLISION_PROB) {
            // Collision occurred
            printf("Collision detected for User %d. Retrying...\n", user->id);
            user->delay = rand() % MAX_DELAY + 1;
            user->has_data = 1;
        } else {
            // Successful transmission
            printf("User %d successfully transmitted data.\n", user->id);
            user->has_data = 0;
        }
    }
}

int main() {
    srand(time(0));
    User users[NUM_USERS];
    
    // Initialize users
    for (int i = 0; i < NUM_USERS; i++) {
        users[i].id = i + 1;
        users[i].has_data = 1;
        users[i].delay = 0;
    }

    printf("Starting Pure ALOHA Protocol Simulation...\n");

    for (int time = 0; time < TIME_FRAME; time++) {
        for (int i = 0; i < NUM_USERS; i++) {
            if (users[i].delay > 0) {
                users[i].delay--;
            } else {
                transmit(&users[i]);
            }

            // Randomly generate new data for the user to transmit
            if (rand() % 10 < 2) {  // 20% chance to have new data
                users[i].has_data = 1;
            }
        }

        usleep(50000);  // Delay to slow down the simulation
    }

    printf("Simulation complete.\n");
    return 0;
}
