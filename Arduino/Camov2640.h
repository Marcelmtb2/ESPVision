#ifndef CAMOV2640_H_  // code guard, avoid multiple inclusions
#define CAMOV2640_H_

#include "esp_camera.h"
#include <Arduino.h>


// Camera module in a ESP32-CAM
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5

#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
// 4 for flash led or 33 for normal led
#define LED_GPIO_NUM 33


// namespace base64 { String encode(const uint8_t*, size_t); }


// Class Definitions
class CamOv2640 {
public:
  CamOv2640() = default;  // Constructor
  ~CamOv2640();

  // Initialize the camera.  Returns true on success.
  bool initCam();

  // Grab a raw frame buffer.  Caller must call esp_camera_fb_return().
  camera_fb_t* captureRaw();
  void returnRaw(camera_fb_t* fb);         // release it

  // Change resolution on the fly.
  bool setResolution(framesize_t size);

  // Change special effect on the fly.
  bool setSpecialEffect(int effect);

  // Change camera configuration to QVGA Grayscale on the fly.
  bool captureQVGA_Grayscale(const char* filename = nullptr);

  // Change camera configuration to UXGA RGB888 on the fly.
  bool captureUXGA_RGB888(const char* filename = nullptr);

  // Base64-encode and return a JPEG buffer.  Also returns the fb to the driver.
  String captureBase64();

  // Just Base64-encode an already-captured fb (and return it afterwards).
  String encodeBase64(camera_fb_t* fb);
private:
  bool initializeCamera_();
};


//     String captureAndConvertToBase64(bool saveToSD, const char* filename);
//     // void saveImageToSD(const char* filename);
//     camera_fb_t* captureRaw();
//     String encodeBase64(camera_fb_t*);
//     bool setResolution(framesize_t sz);
//     bool setEffect(sde_t effect);

//     bool captureQVGA_Grayscale(const char* filename = nullptr);
//     bool captureUXGA_RGB888(const char* filename = nullptr);

//   private:
//     bool _sd_ok = false;
//     bool _initialized {false};
//     bool initialize_camera_();

// };

#endif  //CAMOV2640_H_