#ifndef EDGEVISION_H
#define EDGEVISION_H

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "frontend/webpage.h"

// Pin configuration for ESP32-CAM
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22
#define STREAM_PORT 5000

class EdgeVision {
public:
    EdgeVision();
    void initCamera();
    void startCameraServer();
    void initWiFi(const char* ssid, const char* psw);
    void keepServerAlive();

private:
    WebServer server;  // Create a web server on port 5000
};

#endif  // EDGEVISION_H
