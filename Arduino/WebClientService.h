#ifndef HTTPCLIENTSERVICE_H
#define HTTPCLIENTSERVICE_H

#include <Arduino.h>
#include <HTTPClient.h>

class HTTPClientService {
public:
    HTTPClientService(const char* serverUrl, uint16_t port);

    ~HTTPClientService();

    // Post a single image buffer under a multipart field name
    bool postImage(const char* fieldName,
                   const uint8_t* data, size_t len);

    // Convenience: post all three images in one go
    bool postAll(const char* grayField,
                 const uint8_t* grayData, size_t grayLen,
                 const char* maskField,
                 const uint8_t* maskData, size_t maskLen,
                 const char* uxgaField,
                 const uint8_t* uxgaData, size_t uxgaLen);

private:
    const char* _serverUrl;
    uint16_t _port;
    String      _url;
    String _boundary = "----ESP32FormBoundary";

    void    initBoundary();
    String  partHeader(const char* name, const char* filename);
    String  partFooter();
};
#endif // HTTPCLIENTSERVICE_H
