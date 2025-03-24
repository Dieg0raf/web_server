#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "HttpRequest.h"
#include "HttpResponse.h"

#define PORT 8080

int server_fd = -1;

void cleanup(int signum) {
  if (server_fd != -1) {
    close(server_fd);
    std::cout << "Server socket closed gracefully.\n";
  }
  std::exit(0); // Exit the program
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
  if (bind(server_fd, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) == -1) {
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
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
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
    // TODO: Read more than 1024 bytes (will be more for sure at times) 
    // Use while loop when recv (will probably be the same as "read()"
    char buffer[4096] = {0};
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
      std::cout << "\nReceived: " << buffer << "\n";
    }

    // creates HttpRequest object (holds data of the client requesting)
    HttpRequest *client = new HttpRequest(buffer);

    std::cout << "Client method: " << client->getMethod() << std::endl;
    std::cout << "Client uri: " << client->getUri() << std::endl;
    std::cout << "Client version: " << client->getVersion() << std::endl;
    std::cout << "Client headers: " << client->getHeaders() << std::endl;

    // creates HttpResponse object (holds data for the response I'm going to send)
    HttpResponse *response = new HttpResponse();
    response->setStatus(200);
    response->setContentType("text/plain");
    response->setBody("Your request was received.");

    // Echo the message back to the client
    /*const char *response = "HTTP/1.1 200 OK\r\n"*/
    /*                       "Content-Type: text/plain\r\n"*/
    /*                       "Connection: close\r\n"*/
    /*                       "\r\n"*/
    /*                       "Hello, Client! Your request was received.\r\n";*/

    std::string responseBody = response->getResponse();
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
