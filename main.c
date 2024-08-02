#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h" // Platform-specific includes

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(socket_t client_socket);

int main()
{
    socket_t server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(client_addr);

#ifdef _WIN32
    WSADATA wsa;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
#else
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
#endif

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

#ifdef _WIN32
    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == SOCKET_ERROR)
    {
        printf("Listen failed with error code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
#else
    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
#endif

    printf("Server is running on port %d\n", PORT);

    while (1)
    {
#ifdef _WIN32
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == INVALID_SOCKET)
        {
            printf("Accept failed with error code : %d\n", WSAGetLastError());
            continue;
        }
#else
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }
#endif
        handle_request(client_socket);

#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
    }

#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif
    return 0;
}

void handle_request(socket_t client_socket)
{
    char buffer[BUFFER_SIZE];
#ifdef _WIN32
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
#else
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
#endif
    if (bytes_received < 0)
    {
        perror("Failed to read request");
        return;
    }
    buffer[bytes_received] = '\0';
    printf("Received request:\n%s\n", buffer);

    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!";
#ifdef _WIN32
    send(client_socket, response, strlen(response), 0);
#else
    send(client_socket, response, strlen(response), 0);
#endif
}
