#include "Camov2640.h"
#include "esp_camera.h"
#include "Base64.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "esp32-hal-ledc.h"

#define SPECIAL_EFFECT_NONE 0
#define SPECIAL_EFFECT_NEGATIVE 1
#define SPECIAL_EFFECT_GRAYSCALE 2
#define SPECIAL_EFFECT_RED_TINT 3
#define SPECIAL_EFFECT_GREEN_TINT 4
#define SPECIAL_EFFECT_BLUE_TINT 5
#define SPECIAL_EFFECT_SEPIA 6

CamOv2640::~CamOv2640() {
  esp_camera_deinit();
}


bool CamOv2640::initCam() {
  return initializeCamera_();
}


bool CamOv2640::initializeCamera_() {
  //Set the camera configuration constants
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_UXGA;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: error 0x%x", err);
    return false;
  }

  Serial.println("Camera setup completed");

  return true;
}


camera_fb_t* CamOv2640::captureRaw() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) Serial.println("captureRaw: failed");
  return fb;
}


String CamOv2640::captureBase64() {
  camera_fb_t* fb = captureRaw();
  if (!fb) return "";
  String b64 = base64::encode(fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return b64;
}

String CamOv2640::encodeBase64(camera_fb_t* fb) {
  if (!fb) return "";
  String b64 = base64::encode(fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return b64;
}


bool CamOv2640::setResolution(framesize_t sz) {
  sensor_t* s = esp_camera_sensor_get();
  if (!s) return false;
  // return s->set_framesize(s, sz) == 0;
  s->set_framesize(s, sz);
  delay(100);
  return true;
}


bool CamOv2640::setSpecialEffect(int effect) {
  sensor_t* s = esp_camera_sensor_get();
  // return s && s->set_special_effect(s, effect) == 0;
  s->set_special_effect(s, effect);
  delay(100);
  return true;
}


bool CamOv2640::captureQVGA_Grayscale(const char* filename) {
  if (!setResolution(FRAMESIZE_QVGA)) return false;
  if (!setSpecialEffect(SPECIAL_EFFECT_GRAYSCALE)) return false;

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture QVGA grayscale image");
    return false;
  }

  esp_camera_fb_return(fb);
  return true;
}


bool CamOv2640::captureUXGA_RGB888(const char* filename) {
  if (!setResolution(FRAMESIZE_UXGA)) return false;
  if (!setSpecialEffect(SPECIAL_EFFECT_NONE)) return false;

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture UXGA RGB888 image");
    return false;
  }

  esp_camera_fb_return(fb);
  return true;
}

void CamOv2640::returnRaw(camera_fb_t* fb) {
  esp_camera_fb_return(fb);
}

// String CamOv2640::captureAndConvertToBase64(bool saveToSD, const char* filename) {
//must give the name of the output String variable to be modified by this method
// bool CamOv2640::captureAndConvertToBase64(String& outBase64, const char* filename) {
//   camera_fb_t* fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Camera capture failed");
//       return false;
//     }

//     outBase64 = encodeBase64(fb->buf, fb->len);

//     esp_camera_fb_return(fb);
//     return true;

// }