#ifndef EDGESENSE_H
#define EDGESENSE_H

#include "EdgeSensor.h"
#include "EdgeStream.h"
#include "EdgeStreamWireless.h"
#include "EdgeVision.h"

class EdgeSense {
public:
    EdgeSensor sensor;
    EdgeStream stream;
    EdgeStreamWireless streamWireless;
    EdgeVision vision;

    // Pass i2s_port to EdgeSensor constructor
    EdgeSense(i2s_port_t i2s_port = I2S_NUM_0);

    // Stream
    void beginStream(int baud_rate = 115200);
    void logData(const char* label, std::vector<double> values);

    // Wireless
    void beginWireless(const char* ssid, const char* password);
    void logDataWireless(const char* label, std::vector<double> values);

    // Sensor
    void beginMic(int buff_len = 64);
    void setPins(int sck_pin, int ws_pin, int sd_pin);
    float readMeanAudio();

    // Vision
    void initCamera();
    void startCameraServer();
    void initWiFi(const char* ssid, const char* psw);
    void keepServerAlive();
};

#endif // EDGESENSE_H
