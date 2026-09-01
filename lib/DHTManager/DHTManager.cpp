/******************************************************************************
 * SentinelAI-X
 * File        : DHTManager.cpp
 * Module      : DHT11 Temperature and Humidity Manager
 * Description : Reads temperature and humidity and determines temperature level.
 ******************************************************************************/

#include "DHTManager.h"


//==============================================================================
// Constructor
//==============================================================================

DHTManager::DHTManager() : dht(DHT_PIN, DHT_TYPE)
{
    temperature = 0.0;

    humidity = 0.0;

    temperatureStatus = TemperatureStatus::TEMP_NORMAL;
}


//==============================================================================
// Initialize DHT11
//==============================================================================

void DHTManager::begin()
{
    dht.begin();

    temperature = 0.0;

    humidity = 0.0;

    temperatureStatus = TemperatureStatus::TEMP_NORMAL;
}


//==============================================================================
// Read Temperature and Humidity
//==============================================================================

void DHTManager::update()
{

    temperature = dht.readTemperature();

    humidity = dht.readHumidity();


    // Check sensor error

    if(isnan(temperature) || isnan(humidity))
    {
        return;
    }


    // Temperature classification

    if(temperature < TEMP_HIGH_THRESHOLD)
    {
        temperatureStatus = TemperatureStatus::TEMP_NORMAL;
    }


    else if(temperature < TEMP_CRITICAL_THRESHOLD)
    {
        temperatureStatus = TemperatureStatus::TEMP_HIGH;
    }


    else
    {
        temperatureStatus = TemperatureStatus::TEMP_CRITICAL;
    }

}


//==============================================================================
// Return Temperature Status
//==============================================================================

TemperatureStatus DHTManager::getTemperatureStatus() const
{
    return temperatureStatus;
}


//==============================================================================
// Return Temperature
//==============================================================================

float DHTManager::getTemperature() const
{
    return temperature;
}


//==============================================================================
// Return Humidity
//==============================================================================

float DHTManager::getHumidity() const
{
    return humidity;
}