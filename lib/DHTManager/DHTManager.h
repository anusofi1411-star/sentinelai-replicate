#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : DHTManager.h
 * Module      : DHT11 Temperature and Humidity Manager
 ******************************************************************************/

#include "common.h"
#include <DHT.h>


class DHTManager
{

public:

    // Constructor
    DHTManager();


    // Initialize DHT11 sensor
    void begin();


    // Read temperature and humidity
    void update();


    // Return temperature status
    TemperatureStatus getTemperatureStatus() const;


    // Return temperature value
    float getTemperature() const;


    // Return humidity value
    float getHumidity() const;


private:

    DHT dht;


    float temperature;

    float humidity;


    TemperatureStatus temperatureStatus;

};


#endif // DHT_MANAGER_H