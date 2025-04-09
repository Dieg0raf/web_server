#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// TODO: include <filesystem>

#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ServerSocket.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024 * 1024  // 1MB

int server_fd = -1;
int client_fd = -1;
ServerSocket *serverSocket = nullptr;

void cleanup(int signum) {
    if (serverSocket != nullptr) {
        serverSocket->closeSocket();
        std::cout << "Server socket closed gracefully.\n";
    }

    if (client_fd != -1) {
        if (close(client_fd) < 0) {
            std::cerr << ("Error closing socket") << std::endl;
        }
        std::cout << "Client socket closed gracefully.\n";
    }
    std::exit(0);  // Exit the program
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanup);

    // create Server socket
    try {
        serverSocket = new ServerSocket(PORT);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";
    while (true) {
        client_fd = serverSocket->acceptClient();

        // Receive data from the client
        HttpRequest *client = new HttpRequest(client_fd);
        if (!client->parseRequest()) {
            std::cerr << "Error parsing request\n";
            delete client;
            continue;
        }

        std::cout << "Client method: " << client->getMethod() << std::endl;
        std::cout << "Client uri: " << client->getUri() << std::endl;
        std::cout << "Client version: " << client->getVersion() << std::endl;
        std::cout << "Client headers: " << client->getHeaders() << std::endl;

        // creates HttpResponse object
        HttpResponse *response = new HttpResponse(client_fd);
        response->setStatus(200);
        response->setContentType("text/plain");
        response->setBody("After fixing up the classes!");

        // Echo the message back to the client
        std::string responseBody = response->getResponse();
        std::cout << "Response: " << responseBody << std::endl;
        ssize_t bytes_sent = write(client_fd, responseBody.c_str(), responseBody.size());
        if (bytes_sent == -1) {
            std::cerr << "Error sending response\n";
        }

        delete client;
        delete response;
    }

    close(server_fd);
    return 0;
}
