#ifndef HTTP_RESPONSE_H 
#define HTTP_RESPONSE_H 

#include <unordered_map>
#include <sstream>
#include <string>

class HttpResponse {
public:
  void setHeader(const std::string &key, const std::string &value);
  void setContentType(const std::string &type);
  void setStatus(int newStatus);
  void setBody(const std::string &data);
  std::string getResponse();

private:
  std::string statusToString();
  std::unordered_map<std::string, std::string> headers;
  std::string body;
  std::string contentType;
  int status;
};

#endif
