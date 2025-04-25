#include "HttpRequest.h"

#include <sstream>
#include <string>

#include "Logger.h"

HttpRequest::HttpRequest(const int client_fd) : httpHandler(new HttpHandler(client_fd)) {
}

std::string HttpRequest::getMethod() {
    return method;
}
std::string HttpRequest::getUri() {
    return uri;
}
std::string HttpRequest::getVersion() {
    return version;
}
std::string HttpRequest::getBody() {
    return body;
}

std::string HttpRequest::getHeaders() {
    std::stringstream headers_str;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        headers_str << it->first << ": " << it->second << "\r\n";
    }
    headers_str << "\r\n";
    if (body.size()) {
        headers_str << body;
    }

    return headers_str.str();
}

bool HttpRequest::parseRequest() {
    return parse();
}

bool HttpRequest::parse() {
    try {
        // Accept request (request data from httpHandler)
        if (!httpHandler->receiveRequest()) {
            Logger::getLogger().error("Failed to receive request from client");
            return false;
        }

        // Get request data as string
        std::string requestDataStr = httpHandler->getRequestData();
        Logger::getLogger().debug("Received request data: " + requestDataStr.substr(0, 100) +
                                  (requestDataStr.length() > 100 ? "..." : ""));

        std::istringstream stream(requestDataStr);
        std::string line;

        // Check if invalid request
        if (!std::getline(stream, line)) {
            Logger::getLogger().error("Invalid HTTP request: Empty request");
            return false;
        }

        // Parse request line (method, URI, version)
        std::istringstream request_line(line);
        if (!(request_line >> method >> uri >> version)) {
            Logger::getLogger().error("Invalid request line: " + line);
            return false;
        }

        Logger::getLogger().info("Processing " + method + " request for " + uri);

        // Parse headers
        while (std::getline(stream, line) && !line.empty() && line != "\r") {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            size_t separator = line.find(": ");
            if (separator != std::string::npos) {
                std::string header_name = line.substr(0, separator);
                std::string header_value = line.substr(separator + 2);
                headers[header_name] = header_value;
                Logger::getLogger().debug("Header: " + header_name + " = " + header_value);
            }
        }

        // Parse body
        std::string body_line;
        while (std::getline(stream, body_line)) {
            body += body_line + "\n";
        }

        // Trim trailing newline from body
        if (!body.empty() && body.back() == '\n') {
            body.pop_back();
        }

        if (!body.empty()) {
            Logger::getLogger().debug("Request has body of length: " + std::to_string(body.size()));
        }

        return true;

    } catch (const std::exception& e) {
        Logger::getLogger().error("Exception while parsing request: " + std::string(e.what()));
        return false;
    }
}
