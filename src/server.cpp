#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <memory>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"
#include "ResourceHandler.h"
#include "ServerSocket.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024 * 1024  // 1MB
#define STATIC_PATH "/Users/diegoo/Desktop/Programming/portfolio_projects/web_server/static"

int server_fd = -1;
int client_fd = -1;
std::unique_ptr<ServerSocket> serverSocket = nullptr;

void cleanup(int signum) {
    Logger::getLogger().info("Shutting down server...");
    if (serverSocket != nullptr) {
        serverSocket->closeSocket();
        serverSocket.reset();
        Logger::getLogger().info("Server socket closed gracefully.");
    }

    if (client_fd != -1) {
        if (close(client_fd) < 0) {
            Logger::getLogger().error("Error closing client socket");
        } else {
            Logger::getLogger().info("Client socket closed gracefully.");
        }
    }
    std::exit(0);
}

void handleClient(int client) {
    Logger::getLogger().info("Handling new client connection " + std::to_string(client));
    std::unique_ptr<HttpRequest> request = std::make_unique<HttpRequest>(client);
    std::unique_ptr<HttpResponse> response = std::make_unique<HttpResponse>(client);
    if (!request->parseRequest()) {  // accepts the request and parses it
        Logger::getLogger().error("Error parsing request");
        return;
    }

    // Process response
    std::unique_ptr<ResourceHandler> resourceHandler = std::make_unique<ResourceHandler>(STATIC_PATH);
    resourceHandler->processRequest(request.get(), response.get());

    if (!response->sendResponse()) {
        Logger::getLogger().error("Error sending response");
    } else {
        Logger::getLogger().info("Response sent successfully to client " + std::to_string(client));
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanup);
    if (DEBUG) {
        Logger::getLogger().info("Debug mode is enabled.");
    }

    try {
        serverSocket = std::make_unique<ServerSocket>(PORT);
    } catch (const std::exception &e) {
        Logger::getLogger().error("Error creating server socket: " + std::string(e.what()));
        return 1;
    }

    Logger::getLogger().info("Server is listening on port " + std::to_string(PORT));
    // Listen for incoming requests
    while (true) {
        client_fd = serverSocket->acceptClient();
        handleClient(client_fd);
        client_fd = -1;
    }

    close(server_fd);
    return 0;
}
