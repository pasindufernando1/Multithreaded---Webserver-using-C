#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>  // Signal handling

#define DEFAULT_BUFFER_SIZE 8192
#define SERVER_ROOT_PATH "assets/"
#define THREAD_POOL_SIZE 10     //Can be subjected to changes according to the requirement and the server capacity

// Helper functions
#include "helpers/helpers.h"

// Function declarations
int setup_server_socket(int port);
void *client_thread(void *arg);
void send_response(int client_fd, const char *path);
void send_404(int client_fd);
void send_file(int client_fd, int file_fd, const char *mime_type);
void handle_shutdown(int sig);

// Global variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int server_fd;

int main() {
    int port = 8080;  // Default port
    server_fd = setup_server_socket(port);

    // Signal handler for graceful shutdown
    signal(SIGINT, handle_shutdown);

    // Thread pool initialization
    pthread_t thread_pool[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_create(&thread_pool[i], NULL, client_thread, NULL);
        pthread_detach(thread_pool[i]);
    }

    printf("Server running on port %d\n", port);

    while (1) {
        // Accept a new client connection
        struct sockaddr_in address_client;
        socklen_t addr_len = sizeof(address_client);
        int client_fd = accept(server_fd, (struct sockaddr *)&address_client, &addr_len);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Assign client_fd to a thread
        pthread_mutex_lock(&lock);
        pthread_create(&thread_pool[0], NULL, client_thread, (void *)(intptr_t)client_fd);
        pthread_mutex_unlock(&lock);
    }

    close(server_fd);
    return 0;
}

// Function to set up the server socket
int setup_server_socket(int port) {

    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Set socket option failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(port); // Convert port to network byte order

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Return the server socket file descriptor
    return server_fd;
}

// Function to handle a client request
void *client_thread(void *arg) {
    // Get the client file descriptor from the argument
    int client_fd = (intptr_t)arg;

    // Read the request from the client
    char buffer[DEFAULT_BUFFER_SIZE];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("Failed to read from client");
        close(client_fd);
        return NULL;
    }

    // Null-terminate the buffer
    buffer[bytes_read] = '\0';

    // Parse the request
    char method[16], path[256], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    // Check if the path is empty
    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }

    // Send the response to the client
    send_response(client_fd, path);
    close(client_fd);
    return NULL;
}

// Function to send the response to the client
void send_response(int client_fd, const char *path) {
    int file_fd = openFile(SERVER_ROOT_PATH, path);
    if (file_fd < 0) {
        send_404(client_fd);
    } else {
        const char *file_type = get_file_type(path);
        const char *mime_type = getMimeType(file_type);
        send_file(client_fd, file_fd, mime_type);
        close(file_fd);
    }
}

// Function to send a 404 Not Found response
void send_404(int client_fd) {
    const char *not_found_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 45\r\n"
        "\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>";
    if (write(client_fd, not_found_response, strlen(not_found_response)) < 0) {
        perror("Failed to send 404 response");
    }
}

// Function to send a file to the client
void send_file(int client_fd, int file_fd, const char *mime_type) {
    char response_header[256];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "\r\n", mime_type);
    if (write(client_fd, response_header, strlen(response_header)) < 0) {
        perror("Failed to send response header");
        return;
    }

    char buffer[DEFAULT_BUFFER_SIZE];
    ssize_t bytes;
    while ((bytes = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(client_fd, buffer, bytes) < 0) {
            perror("Failed to send file data");
            return;
        }
    }
}

// Signal handler for graceful shutdown
void handle_shutdown(int sig) {
    (void)sig; // Explicitly mark the parameter as unused
    printf("Shutting down server...\n");
    close(server_fd);
    exit(EXIT_SUCCESS);
}
