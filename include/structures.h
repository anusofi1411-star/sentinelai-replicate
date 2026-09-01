#ifndef STRUCTURES_H
#define STRUCTURES_H

/******************************************************************************
 * SentinelAI-X
 * File        : structures.h
 * Description : Common data structures used across the project.
 ******************************************************************************/

#include <Arduino.h>
#include "enums.h"


//==============================================================================
// Sensor Data Structure
//==============================================================================

struct SensorData
{

    //--------------------------------------------------------------------------
    // PIR Motion Sensor
    //--------------------------------------------------------------------------

    MotionStatus motionStatus = MotionStatus::NO_MOTION;



    //--------------------------------------------------------------------------
    // MQ-2 Gas Sensor
    //--------------------------------------------------------------------------

    GasStatus gasStatus = GasStatus::GAS_LOW;

    uint16_t gasADC = 0;



    //--------------------------------------------------------------------------
    // DHT11 Temperature & Humidity
    //--------------------------------------------------------------------------

    float temperature = 0.0f;

    float humidity = 0.0f;

    TemperatureStatus temperatureStatus =
        TemperatureStatus::TEMP_NORMAL;



    //--------------------------------------------------------------------------
    // Security Button
    //--------------------------------------------------------------------------

    // Button press event
    // true only when button is pressed
    bool buttonPressed = false;


    // Alarm acknowledgement / Mute status
    bool alarmAcknowledged = false;
    bool alarmMuted = false;

    // Manual Panic Trigger state
    bool manualPanicTriggered = false;

    // Current security state
    // true  = ARMED
    // false = DISARMED
    bool securityArmed = false;



    //--------------------------------------------------------------------------
    // Time Information
    //--------------------------------------------------------------------------

    TimeMode timeMode = TimeMode::DAY;



    //--------------------------------------------------------------------------
    // Timestamp
    //--------------------------------------------------------------------------

    String timestamp = "";

};



//==============================================================================
// Threat Data Structure
//==============================================================================

struct ThreatData
{

    //--------------------------------------------------------------------------
    // Threat Score (0-100)
    //--------------------------------------------------------------------------

    uint8_t threatScore = 0;



    //--------------------------------------------------------------------------
    // Threat Level
    //--------------------------------------------------------------------------

    ThreatLevel threatLevel =
        ThreatLevel::SAFE;



    //--------------------------------------------------------------------------
    // Alarm / Buzzer State
    //--------------------------------------------------------------------------

    AlarmState alarmState =
        AlarmState::ALARM_OFF;



    //--------------------------------------------------------------------------
    // Threat Reason & Trigger Attribution
    //--------------------------------------------------------------------------

    String reason = "";

    String triggeredBy = "System Normal";

    String overallRisk = "LOW";

    String eventSource = "NONE";

    String eventName = "NONE";

    String eventStatus = "INACTIVE";

    bool alarmMuted = false;

    bool manualPanicTriggered = false;

    //--------------------------------------------------------------------------
    // Score Breakdown
    //--------------------------------------------------------------------------

    int pirScore = 0;

    int gasScore = 0;

    int tempScore = 0;

    int buttonScore = 0;




    //--------------------------------------------------------------------------
    // Timestamp
    //--------------------------------------------------------------------------

    String timestamp = "";

};



//==============================================================================
// System Status Structure
//==============================================================================

struct SystemStatus
{

    // WiFi Status
    WiFiStatus wifiStatus =
        WiFiStatus::DISCONNECTED;



    // Firebase Status
    FirebaseStatus firebaseStatus =
        FirebaseStatus::FIREBASE_DISCONNECTED;



    // Alarm State
    AlarmState alarmState =
        AlarmState::ALARM_OFF;



    // System Ready Flag
    bool systemReady = false;

};



//==============================================================================
// Alert Data Structure
//==============================================================================

struct AlertData
{

    // Threat Level
    ThreatLevel threatLevel =
        ThreatLevel::SAFE;



    // Alert Message
    String message = "";



    // Timestamp
    String timestamp = "";



    // Alert Sent Status
    bool sent = false;

};


#endif // STRUCTURES_H