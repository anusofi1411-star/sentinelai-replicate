/******************************************************************************
 * SentinelAI-X
 * File        : ThreatEngine.cpp
 * Module      : Threat Decision Engine
 * Description : Calculates threat score and alarm decision
 ******************************************************************************/

#include "ThreatEngine.h"


//==============================================================================
// Constructor
//==============================================================================

ThreatEngine::ThreatEngine()
{

    threatData.threatScore = 0;

    threatData.threatLevel =
        ThreatLevel::SAFE;


    threatData.alarmState =
        AlarmState::ALARM_OFF;


    threatData.reason = "";


    threatData.timestamp = "";

}



//==============================================================================
// Initialize Threat Engine
//==============================================================================

void ThreatEngine::begin()
{

    threatData.threatScore = 0;


    threatData.threatLevel =
        ThreatLevel::SAFE;


    threatData.alarmState =
        AlarmState::ALARM_OFF;


    threatData.reason = "";

}



//==============================================================================
// Calculate Threat Data
//==============================================================================

ThreatData ThreatEngine::calculate(
    const SensorData& sensorData
)
{
    threatData.alarmMuted = sensorData.alarmMuted;
    threatData.manualPanicTriggered = sensorData.manualPanicTriggered;

    //==========================================================================
    // DISARMED MODE
    //==========================================================================

    if(sensorData.securityArmed == false)
    {
        threatData.threatScore = 0;
        threatData.pirScore = 0;
        threatData.gasScore = 0;
        threatData.tempScore = 0;
        threatData.buttonScore = 0;
        threatData.threatLevel = ThreatLevel::SAFE;
        threatData.overallRisk = "LOW";
        threatData.alarmState = AlarmState::ALARM_OFF;
        threatData.reason = "System DISARMED";
        threatData.triggeredBy = "System Disarmed";
        threatData.eventSource = "NONE";
        threatData.eventName = "NONE";
        threatData.eventStatus = "INACTIVE";
        threatData.timestamp = sensorData.timestamp;

        return threatData;
    }

    //==========================================================================
    // ARMED MODE - Calculate Score from Sensors
    //==========================================================================

    int score = calculateScore(sensorData);

    if(score < 0)   score = 0;
    if(score > 100) score = 100;

    threatData.threatScore = (uint8_t)score;
    threatData.threatLevel = determineThreatLevel(threatData.threatScore);

    switch (threatData.threatLevel)
    {
        case ThreatLevel::SAFE:       threatData.overallRisk = "LOW"; break;
        case ThreatLevel::SUSPICIOUS: threatData.overallRisk = "MEDIUM"; break;
        case ThreatLevel::HIGH_RISK:  threatData.overallRisk = "HIGH"; break;
        case ThreatLevel::CRITICAL:   threatData.overallRisk = "HIGH"; break;
        case ThreatLevel::EMERGENCY:  threatData.overallRisk = "EMERGENCY"; break;
        default:                      threatData.overallRisk = "LOW"; break;
    }

    // If alarm is muted by user button, override alarm state to OFF to silence buzzer
    if (sensorData.alarmMuted)
    {
        threatData.alarmState = AlarmState::ALARM_OFF;
    }
    else
    {
        threatData.alarmState = determineAlarmState(threatData.threatLevel);
    }

    threatData.timestamp = sensorData.timestamp;

    // Determine Event Source, Event Name, Event Status, Reason, and Trigger Attribution
    bool hasMotion = (sensorData.motionStatus == MotionStatus::MOTION_DETECTED);
    bool hasGas = (sensorData.gasStatus != GasStatus::GAS_LOW);
    bool hasTemp = (sensorData.temperatureStatus != TemperatureStatus::TEMP_NORMAL);
    bool hasBtn = (sensorData.buttonPressed || sensorData.manualPanicTriggered);

    if (hasBtn && (hasMotion || hasGas || hasTemp))
    {
        threatData.eventSource = "MULTIPLE SENSORS";
        threatData.eventName = "COMBINED THREAT";
        threatData.eventStatus = "ACTIVE";
        threatData.reason = "Multiple Sensors Triggered";
        threatData.triggeredBy = "Multiple Sensors (Panic Button + Sensors)";
    }
    else if (hasBtn)
    {
        threatData.eventSource = "PANIC BUTTON";
        threatData.eventName = "MANUAL ALERT";
        threatData.eventStatus = "ACTIVE";
        threatData.reason = "Manual Alert Triggered";
        threatData.triggeredBy = "Manual Panic Button";
    }
    else if (hasMotion)
    {
        threatData.eventSource = "PIR SENSOR";
        threatData.eventName = (sensorData.timeMode == TimeMode::NIGHT) ? "NIGHT INTRUSION" : "DAY MOTION";
        threatData.eventStatus = "ACTIVE";
        threatData.reason = (sensorData.timeMode == TimeMode::NIGHT) ? "PIR Motion Detected" : "Daytime Motion Detected";
        threatData.triggeredBy = (sensorData.timeMode == TimeMode::NIGHT) ? "PIR Motion Sensor (Night Intrusion)" : "PIR Motion Sensor (Day Motion)";
    }
    else if (hasGas)
    {
        threatData.eventSource = "MQ-2 GAS SENSOR";
        threatData.eventName = "GAS LEAK";
        threatData.eventStatus = "ACTIVE";
        threatData.reason = "Gas Leak Detected";
        threatData.triggeredBy = "MQ-2 Gas Sensor (Gas Leak Detected)";
    }
    else if (hasTemp)
    {
        threatData.eventSource = "DHT11 SENSOR";
        threatData.eventName = "TEMPERATURE OVERHEAT";
        threatData.eventStatus = "ACTIVE";
        threatData.reason = "Temperature High";
        threatData.triggeredBy = "DHT11 Sensor (Overheating)";
    }
    else
    {
        threatData.eventSource = "NONE";
        threatData.eventName = "NONE";
        threatData.eventStatus = "INACTIVE";
        threatData.reason = "System Normal (All Clear)";
        threatData.triggeredBy = "System Normal (All Clear)";
    }

    if (sensorData.alarmMuted)
    {
        threatData.triggeredBy = "Alarm Muted by User (Button Press)";
    }

    return threatData;
}

//==============================================================================
// Calculate Score Based On Time Mode
//==============================================================================

int ThreatEngine::calculateScore(
    const SensorData& sensorData
)
{

    int score = 0;



    // Day time calculation

    if(sensorData.timeMode == TimeMode::DAY)
    {

        score += calculateDayThreat(sensorData);

    }


    // Night time calculation

    else
    {

        score += calculateNightThreat(sensorData);

    }



    return score;

}





//==============================================================================
// DAY TIME THREAT CALCULATION
//==============================================================================
//
// Day Logic:
// - Lab normally has people movement
// - PIR motion alone is low risk
// - Combination of events increases threat
//
//==============================================================================

int ThreatEngine::calculateDayThreat(
    const SensorData& sensorData
)
{
    threatData.pirScore = 0;
    threatData.gasScore = 0;
    threatData.tempScore = 0;
    threatData.buttonScore = 0;

    // PIR Motion
    if(sensorData.motionStatus == MotionStatus::MOTION_DETECTED)
    {
        threatData.pirScore = 5;
    }

    // Gas Sensor
    switch(sensorData.gasStatus)
    {
        case GasStatus::GAS_LOW:      threatData.gasScore = 0;  break;
        case GasStatus::GAS_MEDIUM:   threatData.gasScore = 10; break;
        case GasStatus::GAS_HIGH:     threatData.gasScore = 25; break;
        case GasStatus::GAS_CRITICAL: threatData.gasScore = 50; break;
    }

    // Temperature
    switch(sensorData.temperatureStatus)
    {
        case TemperatureStatus::TEMP_NORMAL:   threatData.tempScore = 0;  break;
        case TemperatureStatus::TEMP_HIGH:     threatData.tempScore = 20; break;
        case TemperatureStatus::TEMP_CRITICAL: threatData.tempScore = 40; break;
    }

    // Manual Panic Button
    if(sensorData.buttonPressed || sensorData.manualPanicTriggered)
    {
        threatData.buttonScore = 70;
    }

    return threatData.pirScore + threatData.gasScore + threatData.tempScore + threatData.buttonScore;
}

int ThreatEngine::calculateNightThreat(
    const SensorData& sensorData
)
{
    threatData.pirScore = 0;
    threatData.gasScore = 0;
    threatData.tempScore = 0;
    threatData.buttonScore = 0;

    // PIR Motion
    if(sensorData.motionStatus == MotionStatus::MOTION_DETECTED)
    {
        threatData.pirScore = 25;
    }

    // Gas Sensor
    switch(sensorData.gasStatus)
    {
        case GasStatus::GAS_LOW:      threatData.gasScore = 0;  break;
        case GasStatus::GAS_MEDIUM:   threatData.gasScore = 15; break;
        case GasStatus::GAS_HIGH:     threatData.gasScore = 35; break;
        case GasStatus::GAS_CRITICAL: threatData.gasScore = 60; break;
    }

    // Temperature
    switch(sensorData.temperatureStatus)
    {
        case TemperatureStatus::TEMP_NORMAL:   threatData.tempScore = 0;  break;
        case TemperatureStatus::TEMP_HIGH:     threatData.tempScore = 30; break;
        case TemperatureStatus::TEMP_CRITICAL: threatData.tempScore = 50; break;
    }

    // Manual Panic Button
    if(sensorData.buttonPressed || sensorData.manualPanicTriggered)
    {
        threatData.buttonScore = 70;
    }

    return threatData.pirScore + threatData.gasScore + threatData.tempScore + threatData.buttonScore;
}
//==============================================================================
// Determine Threat Level
//==============================================================================

ThreatLevel ThreatEngine::determineThreatLevel(uint8_t score)
{

    if(score <= 19)
        return ThreatLevel::SAFE;


    else if(score <= 49)
        return ThreatLevel::SUSPICIOUS;


    else if(score <= 79)
        return ThreatLevel::HIGH_RISK;


    else if(score <= 99)
        return ThreatLevel::CRITICAL;


    else
        return ThreatLevel::EMERGENCY;

}



//==============================================================================
// Determine Alarm State
//==============================================================================

AlarmState ThreatEngine::determineAlarmState(
    ThreatLevel level
)
{

    switch(level)
    {

        case ThreatLevel::SAFE:
            return AlarmState::ALARM_OFF;


        case ThreatLevel::SUSPICIOUS:
            return AlarmState::SHORT_BEEP;


        case ThreatLevel::HIGH_RISK:
            return AlarmState::DOUBLE_BEEP;


        case ThreatLevel::CRITICAL:
            return AlarmState::FAST_BEEP;


        case ThreatLevel::EMERGENCY:
            return AlarmState::CONTINUOUS;


        default:
            return AlarmState::ALARM_OFF;

    }

}



//==============================================================================
// Generate Reason
//==============================================================================

String ThreatEngine::generateReason(
    const SensorData& sensorData
)
{
    String reason = (sensorData.timeMode == TimeMode::DAY) ? "DAY | " : "NIGHT | ";

    if(sensorData.securityArmed)
        reason += "ARMED | ";
    else
        reason += "DISARMED | ";

    if(sensorData.motionStatus == MotionStatus::MOTION_DETECTED)
        reason += "Motion Detected | ";
    else
        reason += "No Motion | ";

    if(sensorData.gasStatus == GasStatus::GAS_CRITICAL)
        reason += "Gas Critical | ";
    else if(sensorData.gasStatus == GasStatus::GAS_HIGH)
        reason += "Gas High | ";
    else if(sensorData.gasStatus == GasStatus::GAS_MEDIUM)
        reason += "Gas Medium | ";
    else
        reason += "Gas Normal | ";

    if(sensorData.temperatureStatus == TemperatureStatus::TEMP_CRITICAL)
        reason += "Temperature Critical";
    else if(sensorData.temperatureStatus == TemperatureStatus::TEMP_HIGH)
        reason += "Temperature High";
    else
        reason += "Temperature Normal";

    return reason;
}



//==============================================================================
// Get Threat Data
//==============================================================================

ThreatData ThreatEngine::getThreatData() const
{

    return threatData;

}