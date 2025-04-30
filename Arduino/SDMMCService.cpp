#include "SDMMCService.h"


// Defines
// SD Card Module
#define SD_CLK_GPIO_NUM 14
#define SD_CMD_GPIO_NUM 15
#define SD_DATA0_GPIO_NUM 2
#define SD_DATA1_GPIO_NUM 4
#define SD_DATA2_GPIO_NUM 12
#define SD_DATA3_GPIO_NUM 13


bool SDMMCService::begin() {
  if (!SD_MMC.setPins(SD_CLK_GPIO_NUM,
                      SD_CMD_GPIO_NUM,
                      SD_DATA0_GPIO_NUM,
                      SD_DATA1_GPIO_NUM,
                      SD_DATA2_GPIO_NUM,
                      SD_DATA3_GPIO_NUM)) {
    Serial.println("Pin change failed!");
    return false;
  }
  if (!SD_MMC.begin()) {
    Serial.println("SD_MMC.begin() failed");
    return false;
  }
  Serial.println("SD card mounted.");
  return true;
}

void SDMMCService::listDir(const char* path, uint8_t levels) {
  Serial.printf("ListDir %s (levels=%u)\n", path, levels);
  File root = SD_MMC.open(path);
  if (!root || !root.isDirectory()) {
    Serial.println("  ! not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

// Optional helper if you want to factor out printing logic
void SDMMCService::_printFileInfo(fs::File& file) {
  Serial.print("    ");
  if (file.isDirectory()) {
    Serial.print("DIR ");
  } else {
    Serial.print("FILE");
  }
  Serial.printf(" %-20s %7u\n", file.name(), file.size());
}


bool SDMMCService::writeFile(const char* path, const uint8_t* data, size_t len) {
  File file = SD_MMC.open(path, FILE_WRITE);
  if (!file) {
    Serial.printf("SDMMCService: failed to open %s for write\n", path);
    return false;
  }
  size_t written = file.write(data, len);
  file.close();
  if (written != len) {
    Serial.printf("SDMMCService: write incomplete (%u of %u)\n", written, len);
    return false;
  }
  return true;
}

int SDMMCService::readFile(const char* path, uint8_t* buffer, size_t bufSize) {
  File file = SD_MMC.open(path);
  if (!file) {
    Serial.printf("SDMMCService: failed to open %s for read\n", path);
    return -1;
  }
  size_t toRead = min(file.size(), bufSize);
  int actuallyRead = file.read(buffer, toRead);
  file.close();
  return actuallyRead;
}


bool SDMMCService::saveFile(const char* path, const uint8_t* data, size_t len) {
  File f = SD_MMC.open(path, FILE_WRITE);
  if (!f) return false;
  f.write(data, len);
  f.close();
  return true;
}


bool SDMMCService::removeFile(const char* path) {
  if (!SD_MMC.remove(path)) {
    Serial.printf("SDMMCService: failed to delete %s\n", path);
    return false;
  }
  return true;
}
