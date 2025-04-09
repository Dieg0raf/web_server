#include "HttpResponse.h"

#include <sstream>
#include <string>

HttpResponse::HttpResponse(const int client_fd) : httpHandler(new HttpHandler(client_fd)) {
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
    headers[key] = value;
}

void HttpResponse::setContentType(const std::string &type) {
    contentType = type;
}

void HttpResponse::setStatus(int newStatus) {
    status = newStatus;
}

void HttpResponse::setBody(const std::string &data) {
    body = data;
}

std::string HttpResponse::statusToStr() {
    return std::to_string(status);
}

std::string HttpResponse::getResponseStr() {
    std::stringstream out;
    setHeader("Content-Type", contentType);
    setHeader("Content-Length", std::to_string(body.size()));
    out << "HTTP/1.1 " << status << " " << statusToStr() << "\r\n";
    for (auto iter = headers.begin(); iter != headers.end(); iter++) {
        out << iter->first << ": " << iter->second << "\r\n";
    }
    out << "\r\n";  // added an empty line to separate headers from body
    if (body.size()) {
        out << body;
    }

    return out.str();
}

bool HttpResponse::sendResponse() {
    std::string response = getResponseStr();
    return httpHandler->sendData(response);
}
