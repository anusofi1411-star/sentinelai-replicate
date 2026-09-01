#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : SensorManager.h
 * Module      : Sensor Manager
 * Description : Collects all sensor data into SensorData structure
 ******************************************************************************/

//==============================================================================
// Common Header
//==============================================================================

#include "common.h"


//==============================================================================
// Sensor Modules
//==============================================================================

#include "TimeManager.h"
#include "DHTManager.h"
#include "MQ2Manager.h"
#include "PIRManager.h"
#include "ButtonManager.h"


//==============================================================================
// Sensor Manager Class
//==============================================================================

class SensorManager
{

public:

    //--------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------

    SensorManager();


    //--------------------------------------------------------------------------
    // Initialize Sensor Manager
    //--------------------------------------------------------------------------

    void begin(
        TimeManager* time,
        DHTManager* dht,
        MQ2Manager* mq2,
        PIRManager* pir,
        ButtonManager* button
    );


    //--------------------------------------------------------------------------
    // Update Sensor Data
    //--------------------------------------------------------------------------

    void update(bool isThreatActive = false);


    //--------------------------------------------------------------------------
    // Get Sensor Data
    //--------------------------------------------------------------------------

    SensorData getSensorData() const;



private:

    //--------------------------------------------------------------------------
    // Manager Pointers
    //--------------------------------------------------------------------------

    TimeManager* timeManager;

    DHTManager* dhtManager;

    MQ2Manager* mq2Manager;

    PIRManager* pirManager;

    ButtonManager* buttonManager;


    //--------------------------------------------------------------------------
    // Stored Sensor Data
    //--------------------------------------------------------------------------

    SensorData sensorData;

};


#endif // SENSOR_MANAGER_H