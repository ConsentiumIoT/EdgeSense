#include "EdgeSense.h"

// Pass i2s_port to sensor
EdgeSense::EdgeSense(i2s_port_t i2s_port)
: sensor(i2s_port), stream(), streamWireless(), vision()
{
}

// Stream
void EdgeSense::beginStream(int baud_rate) {
    stream.begin(baud_rate);
}
void EdgeSense::logData(const char* label, std::vector<double> values) {
    stream.logData(label, values);
}

// Wireless
void EdgeSense::beginWireless(const char* ssid, const char* password) {
    streamWireless.beginWireless(ssid, password);
}
void EdgeSense::logDataWireless(const char* label, std::vector<double> values) {
    streamWireless.logDataWireless(label, values);
}

// Sensor
void EdgeSense::beginMic(int buff_len) {
    sensor.beginMic(buff_len);
}
void EdgeSense::setPins(int sck_pin, int ws_pin, int sd_pin) {
    sensor.setPins(sck_pin, ws_pin, sd_pin);
}
float EdgeSense::readMeanAudio() {
    return sensor.readMeanAudio();
}

// Vision
void EdgeSense::initCamera() {
    vision.initCamera();
}
void EdgeSense::startCameraServer() {
    vision.startCameraServer();
}
void EdgeSense::initWiFi(const char* ssid, const char* psw) {
    vision.initWiFi(ssid, psw);
}
void EdgeSense::keepServerAlive() {
    vision.keepServerAlive();
}
