#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024 * 1024  // 1MB

int server_fd = -1;

void cleanup(int signum) {
    if (server_fd != -1) {
        close(server_fd);
        std::cout << "Server socket closed gracefully.\n";
    }
    std::exit(0);  // Exit the program
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanup);

    // create server socket (returns file descriptor)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket unable to be created\n";
        return 1;
    }

    // Define server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the port
    if (bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Binding failed\n";
        close(server_fd);
        return 1;
    }

    // Start listening for connections (max 5 queued connections)
    if (listen(server_fd, 5) == -1) {
        std::cerr << "Listening failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Accept a client connection
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            std::cerr << "Client accept failed\n";
            break;
        }

        // Extract client's IP and port
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        std::cout << "Client connected from IP: " << client_ip << " Port: " << client_port << std::endl;

        // Receive data from the client
        std::unique_ptr<char> buffer(new char[BUFFER_SIZE]);
        ssize_t bytes_received;
        size_t total_bytes = 0;
        std::string full_request_str;
        while (true) {
            bytes_received = recv(client_fd, buffer.get(), BUFFER_SIZE, 0);
            if (bytes_received == 0) {
                std::cout << "Connection closed" << std::endl;
                break;
            }
            if (bytes_received == -1) {
                std::cerr << "Error receiving data: " << errno << std::endl;
                break;
            }

            std::cout << "Bytes received: " << bytes_received << std::endl;
            full_request_str.append(buffer.get(), bytes_received);
            total_bytes += bytes_received;

            // Check if message size exceeds the limit (e.g., 1MB)
            if (total_bytes > MAX_MESSAGE_SIZE) {
                throw std::runtime_error("Message too large");
            }

            // Optional: Check for HTTP end-of-request (e.g., \r\n\r\n)
            if (full_request_str.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }

        std::cout << "Received request:\n" << full_request_str << std::endl;

        // creates HttpRequest object (holds data of the client requesting)
        HttpRequest *client = new HttpRequest(full_request_str);

        std::cout << "Client method: " << client->getMethod() << std::endl;
        std::cout << "Client uri: " << client->getUri() << std::endl;
        std::cout << "Client version: " << client->getVersion() << std::endl;
        std::cout << "Client headers: " << client->getHeaders() << std::endl;

        // creates HttpResponse object (holds data for the response I'm going to
        // send)
        HttpResponse *response = new HttpResponse();
        response->setStatus(200);
        response->setContentType("text/plain");
        response->setBody("Your request was received.");

        // Echo the message back to the client
        std::string responseBody = response->getResponse();
        std::cout << "Response: " << responseBody << std::endl;
        ssize_t bytes_sent = write(client_fd, responseBody.c_str(), responseBody.size());
        if (bytes_sent == -1) {
            std::cerr << "Error sending response\n";
        }

        // Close the connections
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
