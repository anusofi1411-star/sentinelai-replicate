/******************************************************************************
 * SentinelAI-X
 * File        : PIRManager.cpp
 * Module      : PIR Motion Sensor Manager
 * Description : Reads the PIR sensor and updates the motion status.
 ******************************************************************************/

#include "PIRManager.h"

//==============================================================================
// Constructor
//==============================================================================

PIRManager::PIRManager()
{
    motionStatus = MotionStatus::NO_MOTION;
    pirState = false;
}

//==============================================================================
// Initialize PIR Sensor
//==============================================================================

void PIRManager::begin()
{
    pinMode(PIR_PIN, INPUT);

    motionStatus = MotionStatus::NO_MOTION;
    pirState = false;
}

//==============================================================================
// Update PIR Sensor
//==============================================================================

void PIRManager::update()
{
    pirState = digitalRead(PIR_PIN);

    if (pirState)
    {
        motionStatus = MotionStatus::MOTION_DETECTED;
    }
    else
    {
        motionStatus = MotionStatus::NO_MOTION;
    }
}

//==============================================================================
// Return Motion Status
//==============================================================================

MotionStatus PIRManager::getMotionStatus() const
{
    return motionStatus;
}

//==============================================================================
// Check Motion Detection
//==============================================================================

bool PIRManager::isMotionDetected() const
{
    return (motionStatus == MotionStatus::MOTION_DETECTED);
}