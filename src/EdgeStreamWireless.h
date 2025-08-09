#ifndef EDGESTREAMWIRELESS_H
#define EDGESTREAMWIRELESS_H

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <vector>
#include <string>

using namespace std;

class EdgeStreamWireless {
public:
    EdgeStreamWireless();
    void begin(const char* ssid, const char* password);
    void logDataWireless(const char* sensorName, const vector<double>& sensorValues);

private:
    WebServer server;
    String latestSensorName;
    vector<double> latestSensorValues;

    void connectWiFi(const char* ssid, const char* password);
    void handleRoot();
    static void serverTask(void* param); // FreeRTOS task
};

#endif
