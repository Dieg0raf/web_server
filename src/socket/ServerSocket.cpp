#include "ServerSocket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

#define MAX_CONNECTIONS 5

ServerSocket::ServerSocket(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Failed to create server socket");
    }

    // Define server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the port
    if (bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        throw std::runtime_error("Failed to bind socket to port");
        close(server_fd);
    }

    // Start listening for connections (max 5 connections)
    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        throw std::runtime_error("Listening failed");
        close(server_fd);
    }
}

int ServerSocket::acceptClient() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    if (server_fd < 0) {
        throw std::runtime_error("Server socket is not set up");
    }
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        throw std::runtime_error("Failed to accept");
    }
    return client_fd;
}

ServerSocket::~ServerSocket() {
    closeSocket();
}

void ServerSocket::closeSocket() {
    if (server_fd > 0) {
        close(server_fd);
        server_fd = -1;
    }
}
