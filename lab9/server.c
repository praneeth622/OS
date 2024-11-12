#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 8081
char findClass(int firstOctet)
{
    if (firstOctet >= 1 && firstOctet <= 127)
    {
        return 'A';
    }
    else if (firstOctet >= 128 && firstOctet <= 191)
    {
        return 'B';
    }
    else if (firstOctet >= 192 && firstOctet <= 223)
    {
        return 'C';
    }
    else if (firstOctet >= 224 && firstOctet <= 239)
    {
        return 'D';
    }
    else if (firstOctet >= 240 && firstOctet <= 255)
    {
        return 'E';
    }
    return 'X';
}
int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address, client_address;
    socklen_t addr_len = sizeof(client_address);
    char client_ip[INET_ADDRSTRLEN];
    char response[50];
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, &addr_len)) < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
    printf("Client connected with IP address: %s\n", client_ip);
    int firstOctet;
    sscanf(client_ip, "%d", &firstOctet);
    char ip_class = findClass(firstOctet);
    snprintf(response, sizeof(response), "IP: %s, Class: %c", client_ip, ip_class);
    send(new_socket, response, strlen(response), 0);
    close(new_socket);
    close(server_fd);
    return 0;
}
