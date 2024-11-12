#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>


#define PORT 8080
#define MAX_CLIENTS 5


int clients[MAX_CLIENTS] = {0}; // Track client sockets
pthread_mutex_t lock; 


void *handle_client(void *socket_desc) {
    int sock = (int)socket_desc;
    int valread;
    char buffer[1024] = {0};
    char *collision_msg = "Collision detected! Please retry later.";
    char *ack_msg = "Data received successfully!";
    srand(time(NULL)); // Initialize random number generator\

    while(1) {
        // Simulate carrier sensing by reading from client
        valread = read(sock, buffer, 1024);
        if(valread <= 0) {
            printf("Client disconnected.\n");
            close(sock);
            pthread_exit(NULL);
        }


        // Check if the channel is busy (simulate collision detection)
        pthread_mutex_lock(&lock);
        int collision = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != sock && clients[i] != 0) {
                collision = 1;
                break;
            }
        }


        if(collision) {
            // Collision detected, send a retry message
            printf("Collision detected! Informing client to retry.\n");
            send(sock, collision_msg, strlen(collision_msg), 0);
            sleep(rand() % 5 + 1); // Random backoff before retry
        } else {
            // No collision, acknowledge successful transmission
            printf("Data received from client: %s\n", buffer);
            send(sock, ack_msg, strlen(ack_msg), 0);
        }
        pthread_mutex_unlock(&lock);
    }
    close(sock);
    free(socket_desc);
    pthread_exit(NULL);
}


int main(int argc, char const *argv[]) {
    int server_fd, new_socket, c;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);


    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);


    pthread_mutex_init(&lock, NULL);


    while (1) {
        // Accept new client connections
        if ((new_socket = accept(server_fd, (struct sockaddr )&address, (socklen_t)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New client connected\n");


        // Allocate memory for new client
        pthread_t client_thread;
        int *new_sock = malloc(1);
        *new_sock = new_socket;


        // Add client to the clients array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = new_socket;
                break;
            }
        }


        // Create thread to handle client
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }


        // Detach thread so it cleans up automatically
        pthread_detach(client_thread);
    }


    pthread_mutex_destroy(&lock);
    return 0;
}
