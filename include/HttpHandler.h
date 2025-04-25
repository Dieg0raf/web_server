#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <string>

class HttpHandler {
   public:
    explicit HttpHandler(const int _client_fd);

    // Used for Request
    std::string receiveData();
    bool receiveRequest();
    const std::string& getRequestData();

    // Used for Response
    bool sendData(const std::string& response);
    const int getClientFd();

   private:
    int client_fd;
    std::string full_request_str;
};

#endif
