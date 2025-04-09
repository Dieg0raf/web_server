#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

class ServerSocket {
   public:
    ServerSocket(int port);
    ~ServerSocket();
    int acceptClient();
    void closeSocket();

   private:
    int server_fd;
};

#endif
