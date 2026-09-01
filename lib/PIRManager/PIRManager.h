#ifndef PIR_MANAGER_H
#define PIR_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : PIRManager.h
 * Module      : PIR Motion Sensor Manager
 * Description : Handles PIR sensor initialization and motion detection.
 ******************************************************************************/

#include <common.h>

class PIRManager
{
public:

    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
    PIRManager();

    //-------------------------------------------------------------------------
    // Initialize PIR sensor
    //-------------------------------------------------------------------------
    void begin();

    //-------------------------------------------------------------------------
    // Read PIR sensor and update motion status
    //-------------------------------------------------------------------------
    void update();

    //-------------------------------------------------------------------------
    // Get current motion status
    //-------------------------------------------------------------------------
    MotionStatus getMotionStatus() const;

    //-------------------------------------------------------------------------
    // Returns true if motion is detected
    //-------------------------------------------------------------------------
    bool isMotionDetected() const;

private:

    // Current motion status
    MotionStatus motionStatus;

    // Raw PIR sensor state
    bool pirState;
};

#endif // PIR_MANAGER_H