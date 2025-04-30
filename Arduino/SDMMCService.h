#ifndef ESP32CAM_SDMMCSERVICE_H_  // code guard, avoid multiple inclusions
#define ESP32CAM_SDMMCSERVICE_H_

#include "Arduino.h"

// SD_MMC module
#include "FS.h"
#include "SD_MMC.h"


// A simple service for reading/writing files on any FS implementation (SD_MMC, SPIFFS, etc.).
class SDMMCService {
public:
  // Mount the SD-MMC, returns true on success
  bool begin();

  // List files under "/" or any sub-directory
  void listDir(const char *path = "/", uint8_t levels = 0);

  // Write a block of data to a file (overwrites if it exists).
  // Returns true on success.
  bool writeFile(const char* path, const uint8_t* data, size_t len);

  // Read a file into a pre-allocated buffer.
  // Returns number of bytes read, or -1 on error.
  int readFile(const char* path, uint8_t* buffer, size_t bufSize);

  // Delete a file. Returns true on success.
  bool removeFile(const char* path);

  bool saveFile(const char* path, const uint8_t* data, size_t len);

private:
  void _printFileInfo(fs::File& file);
};

#endif // ESP32CAM_SDMMCSERVICE_H_