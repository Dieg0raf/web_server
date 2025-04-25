#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"

class ResourceHandler {
   public:
    ResourceHandler(const std::string& docRoot);

    void processRequest(HttpRequest* request, HttpResponse* response);

   private:
    void handleGET(HttpResponse* response, const std::string& path);
    bool isValidPath(const std::string& path) const;
    std::string getContentTypeFromExtension(const std::string& path);
    std::string documentRoot;
};
