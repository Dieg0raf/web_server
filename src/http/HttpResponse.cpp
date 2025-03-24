#include "HttpResponse.h"

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
  this->headers[key] = value;
}

void HttpResponse::setContentType(const std::string &type) {
  this->contentType = type;
}

void HttpResponse::setStatus(int newStatus) { this->status = newStatus; }

void HttpResponse::setBody(const std::string &data) { this->body = data; }

std::string HttpResponse::statusToString() {
  return std::to_string(this->status);
}

std::string HttpResponse::getResponse() {
  std::stringstream out;
  setHeader("Content-Type", this->contentType);
  setHeader("Content-Length", std::to_string(this->body.size()));

  out << "HTTP/1.1 " << this->status << " " << statusToString() << "\r\n";
  for (auto iter = headers.begin(); iter != headers.end(); iter++) {
    out << iter->first << ": " << iter->second << "\r\n";
  }
  out << "\r\n";
  if (body.size()) {
    out << body;
  }

  return out.str();
}
