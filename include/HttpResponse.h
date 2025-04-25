#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <memory>
#include <string>
#include <unordered_map>

#include "HttpHandler.h"

class HttpResponse {
   public:
    explicit HttpResponse(const int client_fd);
    ~HttpResponse();
    void setHeader(const std::string &key, const std::string &value);
    void setContentType(const std::string &type);
    void setStatus(int newStatus);
    void setBody(const std::string &data);
    bool sendResponse();

   private:
    std::unique_ptr<HttpHandler> httpHandler;
    std::string statusToStr();
    std::string getResponseStr();
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::string contentType;
    int status;
};

#endif
