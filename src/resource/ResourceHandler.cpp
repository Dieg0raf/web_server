#include "ResourceHandler.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
namespace fs = std::filesystem;

ResourceHandler::ResourceHandler(const std::string& docRoot) : documentRoot(docRoot) {
}

void ResourceHandler::processRequest(HttpRequest* request, HttpResponse* response) {
    // Extract path from request
    std::string path = documentRoot + request->getUri();
    if (path.empty() || path == documentRoot + "/") {
        path += "/index.html";
    }

    // Handle different methods differently
    std::string method = request->getMethod();
    if (method == "GET") {
        handleGET(response, path);
    } else {
        response->setStatus(405);
        response->setContentType("text/plain");
        response->setBody("405 Method Not Allowed");
    }
}

bool ResourceHandler::isValidPath(const std::string& path) const {
    if (path.find("..") != std::string::npos || path.find("../") != std::string::npos) {
        return false;
    }
    if (!fs::is_regular_file(path)) {
        return false;
    }
    if (!fs::exists(path)) {
        return false;
    }
    return true;
}

void ResourceHandler::handleGET(HttpResponse* response, const std::string& path) {
    // Check path traversal
    if (!isValidPath(path)) {
        response->setStatus(404);
        response->setContentType("text/plain");
        response->setBody("404 Not Found");
        return;
    }

    // Copies files binary (easier to keep file state intact when copying)
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        response->setStatus(500);
        response->setContentType("text/plain");
        response->setBody("500 Internal Server Error - Cannot read file");
        return;
    }

    // Read the file into a string
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    response->setStatus(200);
    response->setContentType(getContentTypeFromExtension(path));
    response->setBody(content);
}

std::string ResourceHandler::getContentTypeFromExtension(const std::string& path) {
    fs::path fsPath(path);
    std::string extension = fsPath.extension().string();

    // Convert extension to lowercase and remove the dot
    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "json") return "application/json";
    if (extension == "txt") return "text/plain";
    if (extension == "xml") return "application/xml";
    if (extension == "pdf") return "application/pdf";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    if (extension == "gif") return "image/gif";
    if (extension == "svg") return "image/svg+xml";
    if (extension == "ico") return "image/x-icon";

    // default
    return "application/octet-stream";
}
