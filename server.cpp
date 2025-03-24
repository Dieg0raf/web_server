#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int server_fd = -1;

void cleanup(int signum) {
  if (server_fd != -1) {
    close(server_fd);
    std::cout << "Server socket closed gracefully.\n";
  }
  exit(0); // Exit the program
}

int main(int argc, char *argv[]) {

  // Set up signal handler for SIGINT (Ctrl+C)
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
    /*sockaddr_in client_addr;*/
    /*socklen_t client_len = sizeof(client_addr);*/
    /*int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
     * &client_len);*/

    // Accept a client connection
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd == -1) {
      std::cerr << "Client accept failed\n";
      break;
    }

    std::cout << "Client connected!\n";
    std::cout << "client_fd: " << client_fd << std::endl;

    // Receive data from the client
    char buffer[1024] = {0};
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
      std::cout << "\nReceived: " << buffer << "\n";
    }

    // Echo the message back to the client
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "Hello, Client! Your request was received.\r\n";

    ssize_t bytes_sent = write(client_fd, response, strlen(response));
    if (bytes_sent == -1) {
        std::cerr << "Error sending response\n";
    }

    // Close the connections
    close(client_fd);
  }

  close(server_fd);
  return 0;
}
