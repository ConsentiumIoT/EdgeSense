#include "EdgeStreamWireless.h"

EdgeStreamWireless::EdgeStreamWireless() : server(80) {}

void EdgeStreamWireless::beginWireless(const char* ssid, const char* password) {
    connectWiFi(ssid, password);

    // Define GET route
    server.on("/", HTTP_GET, [this]() { handleRoot(); });

    server.begin();
    Serial.println("HTTP server started");

#if defined(ESP32)
    #if CONFIG_IDF_TARGET_ESP32 // Dual-core ESP32
        xTaskCreatePinnedToCore(
            serverTask,      // Task function
            "ServerTask",    // Name
            4096,            // Stack size
            this,            // Parameter
            1,               // Priority
            NULL,            // Task handle
            1                // Core ID (1 or 0)
        );
    #else // ESP32-C3, ESP32-S2, ESP32-S3 (single-core)
        xTaskCreate(
            serverTask,      // Task function
            "ServerTask",    // Name
            4096,            // Stack size
            this,            // Parameter
            1,               // Priority
            NULL             // Task handle
        );
    #endif
#endif
}

void EdgeStreamWireless::connectWiFi(const char* ssid, const char* password) {
    Serial.printf("Connecting to %s", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void EdgeStreamWireless::logDataWireless(const char* sensorName, const vector<double>& sensorValues) {
    latestSensorName = sensorName;
    latestSensorValues = sensorValues;
    // No Serial printing here anymore
}

void EdgeStreamWireless::handleRoot() {
    DynamicJsonDocument doc(512);

    doc["sensorName"] = latestSensorName;
    JsonArray data = doc.createNestedArray("sensorValues");
    for (double val : latestSensorValues) {
        data.add(val);
    }

    String response;
    serializeJson(doc, response);

    // Serial log only when sending the GET request
    Serial.print("Sending:");
    Serial.println(response);

    server.send(200, "application/json", response);
}

void EdgeStreamWireless::serverTask(void* param) {
    EdgeStreamWireless* self = static_cast<EdgeStreamWireless*>(param);
    for (;;) {
        self->server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms yield
    }
}
