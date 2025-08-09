#include "EdgeStreamWireless.h"

EdgeStreamWireless::EdgeStreamWireless() : server(80) {}

void EdgeStreamWireless::begin(const char* ssid, const char* password) {
    Serial.begin(115200);
    connectWiFi(ssid, password);

    // Define GET route
    server.on("/", HTTP_GET, [this]() { handleRoot(); });

    server.begin();
    Serial.println("HTTP server started");

    // Start background task to handle HTTP requests
    xTaskCreatePinnedToCore(
        serverTask,    // Task function
        "HTTPServer",  // Task name
        4096,          // Stack size
        this,          // Parameter
        1,             // Priority
        nullptr,       // Task handle
        1              // Core (1 = App core, 0 = Pro core)
    );
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
