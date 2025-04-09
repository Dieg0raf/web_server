#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <string>

class HttpHandler {
   public:
    explicit HttpHandler(const int _client_fd);
    ~HttpHandler();

    // Used for Request
    std::string receiveData();
    bool receiveRequest();
    const std::string& getRequestData();

    // Used for Response
    int sendData(const std::string& response);

   private:
    int client_fd;
    std::string full_request_str;
};

#endif
