#ifndef EDGESTREAM_H
#define EDGESTREAM_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

using namespace std;

class EdgeStream{
    public:
        EdgeStream();
        void begin(int baud_rate=115200);
        void logData(const char* , vector<double>);

};


#endif  // EDGESTREAM_H