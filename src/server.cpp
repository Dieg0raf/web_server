#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
/*#include <memory>*/
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
namespace fs = std::filesystem;

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

        std::string uri = "/Users/diegoo/Desktop/Programming/portfolio_projects/web_server" + client->getUri();
        std::cout << "Client method: " << client->getMethod() << std::endl;
        std::cout << "Client uri: " << client->getUri() << std::endl;
        std::cout << "Client version: " << client->getVersion() << std::endl;
        std::cout << "Client headers: " << client->getHeaders() << std::endl;

        // creates HttpResponse object
        HttpResponse *response = new HttpResponse(client_fd);
        if (uri.find("../") != std::string::npos || uri.find("..\\") != std::string::npos) {
            // Path traversal attempt detected
            response->setStatus(404);  // or 403 Forbidden
            response->setContentType("text/plain");
            response->setBody("Path traversal attempt detected!");
        } else {
            if (!fs::exists(uri)) {
                response->setStatus(404);  // Not Found
                response->setContentType("text/plain");
                response->setBody("404 Not Found");
            } else {
                // Read the file content
                std::ifstream file(uri, std::ios::binary);
                if (!file) {
                    response->setStatus(500);  // Internal Server Error
                    response->setContentType("text/plain");
                    response->setBody("500 Internal Server Error - Cannot read file");
                }

                // Read the file into a string
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                // Set response body and status
                response->setStatus(200);  // OK
                response->setContentType("text/html");
                response->setBody(content);
            }
        }

        if (!response->sendResponse()) {
            std::cerr << "Error sending response\n";
        }

        client_fd = -1;  // Reset client_fd for the next iteration
        delete client;
        delete response;
    }

    close(server_fd);
    return 0;
}
