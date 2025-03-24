#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <sstream>
#include <string>
#include <unordered_map>

// Add Http Field to share utils between classes since Http handles the requests
// the same as response
class HttpRequest {
public:
  HttpRequest(const std::string &raw_request);
  std::string getMethod();
  std::string getUri();
  std::string getVersion();
  std::string getHeaders();
  std::string getBody();

private:
  void parse(const std::string &raw_request);
  std::string method;
  std::string uri;
  std::string version;
  std::unordered_map<std::string, std::string> headers;
  std::string body;
};

#endif
