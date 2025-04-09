#include "HttpHandler.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>  // TODO: remove after
#include <memory>
#include <stdexcept>
#include <string>
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024 * 1024  // 1MB

HttpHandler::HttpHandler(const int _client_fd) : client_fd(_client_fd) {
}

HttpHandler::~HttpHandler() {
    if (client_fd > 0) {
        if (close(client_fd) < 0) {
            std::runtime_error("Error closing socket");
        }
        std::cout << "Client " << client_fd << " socket closed gracefully.\n";
    }
}

const std::string& HttpHandler::getRequestData() {
    return full_request_str;
}

// used to receive the request
bool HttpHandler::receiveRequest() {
    try {
        full_request_str = receiveData();
        return !full_request_str.empty();
    } catch (const std::exception& e) {
        return false;
    }
}

// actually receives the data
std::string HttpHandler::receiveData() {
    std::unique_ptr<char> buffer(new char[BUFFER_SIZE]);
    ssize_t bytes_received;
    size_t total_bytes = 0;
    std::string full_request_str = "";
    while (true) {
        bytes_received = recv(client_fd, buffer.get(), BUFFER_SIZE, 0);
        if (bytes_received == 0) {
            std::runtime_error("Connection closed");
            break;
        }
        if (bytes_received == -1) {
            std::runtime_error("Error receiving data");
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
    return full_request_str;
}

// used to send the response
bool HttpHandler::sendData(const std::string& response) {
    ssize_t bytes_sent = write(client_fd, response.c_str(), response.size());
    if (bytes_sent < 0) {
        return false;
    }
    return true;
}
