#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <memory>
#include <string>
#include <unordered_map>

#include "HttpHandler.h"

// Add Http Field to share utils between classes since Http handles the requests
// the same as response
class HttpRequest {
   public:
    explicit HttpRequest(const int client_fd);
    bool parseRequest();
    std::string getMethod();
    std::string getUri();
    std::string getVersion();
    std::string getHeaders();
    std::string getBody();

   private:
    bool parse();
    std::unique_ptr<HttpHandler> httpHandler;
    std::string method;
    std::string uri;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

#endif
