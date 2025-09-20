#include "EdgeVision.h"

EdgeVision::EdgeVision() : server(STREAM_PORT) {  // initialize with port 5000
}

void EdgeVision::initCamera() {
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
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG;  // for streaming
    //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // Initialize camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        ESP.restart();
    } else {
        Serial.println("Camera initialized successfully (safe mode).");
    }

    // Extra tuning at sensor level
    sensor_t *s = esp_camera_sensor_get();
    if (s) {
        s->set_framesize(s, FRAMESIZE_QVGA);
        s->set_quality(s, 30);
        s->set_gain_ctrl(s, 1);      // auto gain
        s->set_exposure_ctrl(s, 1);  // auto exposure
        s->set_brightness(s, 0);
        s->set_saturation(s, 0);
    }
}

void EdgeVision::startCameraServer() {
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", main_page);
    });

    // MJPEG stream
    server.on("/stream", HTTP_GET, [this]() {
        WiFiClient client = server.client();

        const char http_header[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
        client.write((const uint8_t*)http_header, sizeof(http_header) - 1);

        const unsigned long clientTimeoutMs = 10000; // disconnect if idle
        unsigned long lastActivity = millis();

        int dmaErrorCount = 0;

        while (client.connected()) {
            if (millis() - lastActivity > clientTimeoutMs) break;

            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) {
                dmaErrorCount++;
                Serial.println("Frame grab failed (DMA overflow likely).");
                if (dmaErrorCount > 5) {
                    Serial.println("Too many DMA errors, restarting...");
                    ESP.restart();
                }
                delay(50);
                continue;
            }

            // reset error counter
            dmaErrorCount = 0;

            char headerBuf[128];
            int headerLen = snprintf(headerBuf, sizeof(headerBuf),
                                    "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                                    (unsigned)fb->len);
            if (headerLen > 0) client.write((const uint8_t*)headerBuf, (size_t)headerLen);

            const size_t chunkSize = 1024;
            size_t sent = 0;
            while (sent < fb->len) {
                size_t toSend = fb->len - sent;
                if (toSend > chunkSize) toSend = chunkSize;
                size_t wrote = client.write(fb->buf + sent, toSend);
                if (wrote == 0) break; // client dropped
                sent += wrote;
                delay(0);
            }

            client.write((const uint8_t*)"\r\n", 2);
            esp_camera_fb_return(fb);

            lastActivity = millis();

            // limit FPS → ~3–4 fps
            delay(300);
        }

        if (client.connected()) client.stop();
    });

    server.begin();
    Serial.println("Camera server started.");
}

void EdgeVision::initWiFi(const char* ssid, const char* psw) {
    WiFi.begin(ssid, psw);
    WiFi.setSleep(false);  // disable WiFi power save
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.printf("http://%s:%d\n", WiFi.localIP().toString().c_str(), STREAM_PORT);
}

void EdgeVision::keepServerAlive() {
    server.handleClient();
}
