/******************************************************************************
 * SentinelAI-X
 * File        : SensorManager.cpp
 * Module      : Sensor Manager
 * Description : Collects all sensor data into SensorData structure
 ******************************************************************************/

#include "SensorManager.h"


//==============================================================================
// Constructor
//==============================================================================

SensorManager::SensorManager()
{
    timeManager = nullptr;

    dhtManager = nullptr;

    mq2Manager = nullptr;

    pirManager = nullptr;

    buttonManager = nullptr;
}
//==============================================================================
// Initialize Sensor Manager
//==============================================================================

void SensorManager::begin(
    TimeManager* time,
    DHTManager* dht,
    MQ2Manager* mq2,
    PIRManager* pir,
    ButtonManager* button
)
{

    // Store manager references

    timeManager = time;

    dhtManager = dht;

    mq2Manager = mq2;

    pirManager = pir;

    buttonManager = button;

} 
//==============================================================================
// Update Sensor Data
//==============================================================================

void SensorManager::update(bool isThreatActive)
{

    //--------------------------------------------------------------------------
    // Update All Modules
    //--------------------------------------------------------------------------

    timeManager->update();

    dhtManager->update();

    mq2Manager->update();

    pirManager->update();

    buttonManager->update(isThreatActive);



    //--------------------------------------------------------------------------
    // PIR Motion Sensor
    //--------------------------------------------------------------------------

    sensorData.motionStatus =
        pirManager->getMotionStatus();



    //--------------------------------------------------------------------------
    // MQ-2 Gas Sensor
    //--------------------------------------------------------------------------

    sensorData.gasADC =
        mq2Manager->getRawValue();


    sensorData.gasStatus =
        mq2Manager->getGasStatus();



    //--------------------------------------------------------------------------
    // DHT11 Temperature and Humidity
    //--------------------------------------------------------------------------

    sensorData.temperature =
        dhtManager->getTemperature();


    sensorData.humidity =
        dhtManager->getHumidity();


    sensorData.temperatureStatus =
        dhtManager->getTemperatureStatus();
    //--------------------------------------------------------------------------
    // Security Button / ARM-DISARM System & Threat Control
    //--------------------------------------------------------------------------

    // Momentary button press event
    sensorData.buttonPressed =
        buttonManager->isPressed();

    // Current security mode
    sensorData.securityArmed =
        buttonManager->isArmed();

    // Alarm mute state & Manual panic state
    sensorData.alarmMuted =
        buttonManager->isAlarmMuted();

    sensorData.manualPanicTriggered =
        buttonManager->isManualPanicActive();

    sensorData.alarmAcknowledged = sensorData.alarmMuted;



    //--------------------------------------------------------------------------
    // Time Information
    //--------------------------------------------------------------------------

    sensorData.timeMode =
        timeManager->getTimeMode();



    sensorData.timestamp =
        String(timeManager->getDateString()) +
        " " +
        String(timeManager->getTimeString());

}
//==============================================================================
// Get Sensor Data
//==============================================================================

SensorData SensorManager::getSensorData() const
{
    return sensorData;
}


//==============================================================================
// End of SensorManager.cpp
//==============================================================================