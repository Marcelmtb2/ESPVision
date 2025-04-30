#include "WebClientService.h"


HTTPClientService::HTTPClientService(const char* serverUrl, uint16_t port)
 : _serverUrl(serverUrl), _port(port) {
  // build the target URL once
  _url = String("http://") + _serverUrl + ":" + String(_port) + "/upload";
  initBoundary();
}


HTTPClientService::~HTTPClientService() {}


// generate a simple “random” multipart boundary
void HTTPClientService::initBoundary() {
    // simple random-ish boundary
    _boundary = "----ESP32Boundary" + String(micros());
}

String HTTPClientService::partHeader(const char* name, const char* filename) {
    String s = "--" + _boundary + "\r\n";
    s += "Content-Disposition: form-data; name=\"" + String(name) +
        "\"; filename=\"" + String(filename) + "\"\r\n";
    s += "Content-Type: image/jpeg\r\n\r\n";
    return s;
}

String HTTPClientService::partFooter() {
    return "\r\n";
}

bool HTTPClientService::postImage(const char* fieldName,
                                  const uint8_t* data, size_t len)
{
    HTTPClient http;
    http.begin(_url);
    http.addHeader("Content-Type",
                    "multipart/form-data; boundary=" + _boundary);

    // build header + data + footer in one buffer
    String filename = String(fieldName) + ".jpg";
    String header = partHeader(fieldName, filename.c_str());
    String footer = "--" + _boundary + "--\r\n";

    size_t totalLen = header.length() + len + footer.length();
    auto buf = (uint8_t*)malloc(totalLen);
    uint8_t* p = buf;

    memcpy(p, header.c_str(), header.length());         p += header.length();
    memcpy(p, data,                 len);               p += len;
    memcpy(p, footer.c_str(), footer.length());

    int code = http.sendRequest("POST", buf, totalLen);
    free(buf);
    http.end();

    return (code >= 200 && code < 300);
}

bool HTTPClientService::postAll(const char* grayField,
                                const uint8_t* grayData, size_t grayLen,
                                const char* maskField,
                                const uint8_t* maskData, size_t maskLen,
                                const char* uxgaField,
                                const uint8_t* uxgaData, size_t uxgaLen)
{
    // Simplest path: three separate small posts:
    bool ok1 = postImage(grayField, grayData, grayLen);
    bool ok2 = postImage(maskField, maskData, maskLen);
    bool ok3 = postImage(uxgaField, uxgaData, uxgaLen);
    return ok1 && ok2 && ok3;
}
