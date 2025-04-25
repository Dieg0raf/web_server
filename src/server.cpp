#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ResourceHandler.h"
#include "ServerSocket.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024 * 1024  // 1MB
#define STATIC_PATH "/Users/diegoo/Desktop/Programming/portfolio_projects/web_server/static"

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
    std::exit(0);
}

void handleClient(int client) {
    std::unique_ptr<HttpRequest> request = std::make_unique<HttpRequest>(client);
    std::unique_ptr<HttpResponse> response = std::make_unique<HttpResponse>(client);
    if (!request->parseRequest()) {  // accepts the request and parses it
        std::cerr << "Error parsing request\n";
        return;
    }

    // Process response
    ResourceHandler *resourceHandler = new ResourceHandler(STATIC_PATH);
    resourceHandler->processRequest(request.get(), response.get());

    if (!response->sendResponse()) {
        std::cerr << "Error sending response\n";
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanup);

    try {
        serverSocket = new ServerSocket(PORT);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";
    // Listen for incoming requests
    while (true) {
        client_fd = serverSocket->acceptClient();
        handleClient(client_fd);
        client_fd = -1;
    }

    close(server_fd);
    return 0;
}
