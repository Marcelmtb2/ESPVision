#include <Arduino.h>
#include "Camov2640.h"
#include "SDMMCService.h"
#include "WebClientService.h"
#include "FS.h"
#include <WiFi.h>


// // these must match your Camov2640.cpp defines:
#define SPECIAL_EFFECT_NONE      0
#define SPECIAL_EFFECT_GRAYSCALE 2
// ———————————————————————————————
//  CONFIG
// ———————————————————————————————
static const char*  SERVER = "192.168.155.129";
static const uint16_t PORT  = 8000;

// placeholder filenames on your SD card:
const char* PLACEHOLDERS_QVGA[] = { "/qvga0.jpg", "/qvga1.jpg" };
const char* PLACEHOLDERS_MASK[] = { "/mask0.jpg", "/mask1.jpg" };
const char* PLACEHOLDERS_UXGA[] = { "/uxga0.jpg", "/uxga1.jpg" };

CamOv2640        cam;
SDMMCService     sd;
HTTPClientService http(SERVER, PORT);

// A single function to build any endpoint URL:
String makeUrl(const char* path) {
  // e.g. "http://192.168.155.129:8000/upload"
  return String("http://") + SERVER_HOST + ":" + SERVER_PORT + path;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("\n=== BOOT ===");

  // 1) SD card
  if (!sd.begin()) {
    Serial.println("❌ SD init failed, continuing anyway");
  } else {
    Serial.println("✅ SD init OK");
  }

  // 2) Camera
  if (!cam.initCam()) {
    Serial.println("❌ Camera init failed, using placeholders only");
  } else {
    Serial.println("✅ Camera init OK");
  }

  // 3) Decide which placeholders to send this run:
  //    we’ll store a single-byte counter in NVS or RTC memory so
  //    that each reboot flips between 0 and 1.
  uint8_t toggle = 0;
  if (sd.fileExists("/toggle.bin")) {
    File f = sd.openFile("/toggle.bin", FILE_READ);
    toggle = f.read();
    f.close();
  }
  toggle = (toggle + 1) & 0x01;  // flip 0↔1
  {
    File f = sd.openFile("/toggle.bin", FILE_WRITE);
    f.write(toggle);
    f.close();
  }
  Serial.printf("→ placeholder set #%u\n", toggle);

  // 4) Attempt a real camera capture…
  camera_fb_t* fb_qvga = nullptr;
  camera_fb_t* fb_uxga = nullptr;

  if (cam.isInitialized()) {
    // QVGA gray
    cam.setResolution(FRAMESIZE_QVGA);
    cam.setSpecialEffect(SPECIAL_EFFECT_GRAYSCALE);
    delay(50);
    fb_qvga = cam.captureRaw();

    // UXGA color
    cam.setResolution(FRAMESIZE_UXGA);
    cam.setSpecialEffect(SPECIAL_EFFECT_NONE);
    delay(200);
    fb_uxga = cam.captureRaw();
  }

  // 5) Prepare buffers & lengths for POST:
  const uint8_t*  grayBuf; size_t grayLen;
  const uint8_t*  maskBuf; size_t maskLen;
  const uint8_t*  uxgaBuf; size_t uxgaLen;

  if (fb_qvga && fb_uxga) {
    // success: read mask = same as gray for now
    grayBuf = fb_qvga->buf; grayLen = fb_qvga->len;
    maskBuf = fb_qvga->buf; maskLen = fb_qvga->len;
    uxgaBuf = fb_uxga->buf; uxgaLen = fb_uxga->len;
    Serial.println("📸 Using real camera buffers");
  } else {
    // fallback to placeholders on SD card
    Serial.println("📸 Using placeholders from SD");
    File f;

    f = sd.openFile(PLACEHOLDERS_QVGA[toggle], FILE_READ);
    grayLen = f.size();
    grayBuf = (const uint8_t*)malloc(grayLen);
    f.read((uint8_t*)grayBuf, grayLen);
    f.close();

    f = sd.openFile(PLACEHOLDERS_MASK[toggle], FILE_READ);
    maskLen = f.size();
    maskBuf = (const uint8_t*)malloc(maskLen);
    f.read((uint8_t*)maskBuf, maskLen);
    f.close();

    f = sd.openFile(PLACEHOLDERS_UXGA[toggle], FILE_READ);
    uxgaLen = f.size();
    uxgaBuf = (const uint8_t*)malloc(uxgaLen);
    f.read((uint8_t*)uxgaBuf, uxgaLen);
    f.close();
  }

  // 6) Send them all in one go:
  bool ok = http.postAll(
    "gray" , grayBuf, grayLen,
    "mask" , maskBuf, maskLen,
    "uxga" , uxgaBuf, uxgaLen
  );

  Serial.printf("🌐 HTTP POST all → %s\n", ok ? "SUCCESS":"FAIL");

  // 7) Clean up
  if (fb_qvga) esp_camera_fb_return(fb_qvga);
  if (fb_uxga) esp_camera_fb_return(fb_uxga);
  if (!(fb_qvga && fb_uxga)) {
    free((void*)grayBuf);
    free((void*)maskBuf);
    free((void*)uxgaBuf);
  }

  Serial.println("=== Done. You may remove USB ===");
}

void loop() {
  // nothing
}

// #include <Arduino.h>
// #include "Camov2640.h"
// #include "SDMMCService.h"

// // these must match your Camov2640.cpp defines:
// #define SPECIAL_EFFECT_NONE      0
// #define SPECIAL_EFFECT_GRAYSCALE 2

// CamOv2640      cam;
// SDMMCService   sd;


// camera_fb_t* warmupCapture(int discardCount = 5, uint16_t delayMs = 50) {
//   camera_fb_t* fb = nullptr;
//   for (int i = 0; i < discardCount; i++) {
//     fb = cam.captureRaw();         // grab and immediately return
//     if (fb) esp_camera_fb_return(fb);
//     delay(delayMs);
//   }
//   // last real grab — don’t return until after you’ve processed it
//   fb = cam.captureRaw();
//   return fb;
// }





// void setup() {
//   Serial.begin(115200);
//   while (!Serial) { delay(10); }
//   Serial.println("\n=== startup ===");

//   // 1) init SD
//   // if (!sd.begin()) {
//   //   Serial.println("❌ SD init failed");
//   //   return;
//   // }
//   // Serial.println("✅ SD init OK");
//   bool sdOK = sd.begin();
//   if (!sdOK) {
//     Serial.println(F("❌ SD init failed, proceeding without file saves"));
//   } else {
//     Serial.println(F("✅ SD init OK"));
//   }

//   // 2) init camera
//   if (!cam.initCam()) {
//     Serial.println("❌ Camera init failed");
//     return;
//   }
//   Serial.println("✅ Camera init OK");

//   camera_fb_t* fb = nullptr;

//   // --- Capture QVGA Grayscale ---
//   Serial.println("\n--- Capturing QVGA Grayscale ---");
//   cam.setResolution(FRAMESIZE_QVGA);
//   cam.setSpecialEffect(SPECIAL_EFFECT_GRAYSCALE);
//   delay(100);  // let registers update

//   fb = warmupCapture(15, 50);
//   if (fb) {
//     Serial.println(F("📸 Captured QVGA after warm-up"));
//     // e.g. save to SD or send via HTTP…
//     if (sdOK) {
//       sd.writeFile("/qvga.jpg", fb->buf, fb->len);
//       Serial.println(F("💾 QVGA saved"));
//     } else {
//       Serial.println(F("⚠️ skipping SD write"));
//     }
//     esp_camera_fb_return(fb);
//   }
//   // --- Capture UXGA RGB888 ---
//   Serial.println("\n--- Capturing UXGA RGB888 ---");
//   cam.setResolution(FRAMESIZE_UXGA);
//   cam.setSpecialEffect(SPECIAL_EFFECT_NONE);
//     // the core driver may not support RAW RGB888; stick with JPEG for UXGA
//   delay(200);

//   fb = warmupCapture(15, 50);
//   if (fb) {
//     Serial.println(F("📸 Captured UXGA after warm-up"));
//     if (sdOK) {
//       sd.writeFile("/uxga.jpg", fb->buf, fb->len);
//       Serial.println(F("💾 UXGA saved"));
//     } else {
//       Serial.println(F("⚠️ skipping SD write"));
//     }
//     esp_camera_fb_return(fb);
//   }

//   Serial.println("\nAll done. You can remove USB now.");
// }

// void loop() {
//   // nothing here
// }