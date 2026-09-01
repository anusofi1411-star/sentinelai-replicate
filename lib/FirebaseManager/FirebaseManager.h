#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : FirebaseManager.h
 * Module      : Firebase Manager
 * Description : Handles Firebase connection and uploads sensor, threat,
 *               and alert information to the cloud under /Lab2 node.
 ******************************************************************************/

#include "common.h"
#include "TimeManager.h"

class FirebaseManager
{
public:

    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
    FirebaseManager();

    //-------------------------------------------------------------------------
    // Initialize Firebase
    //-------------------------------------------------------------------------
    void begin();

    //-------------------------------------------------------------------------
    // Connect to Firebase
    //-------------------------------------------------------------------------
    bool connect();

    //-------------------------------------------------------------------------
    // Update Firebase connection status
    //-------------------------------------------------------------------------
    void update();

    //-------------------------------------------------------------------------
    // Upload Complete Lab Node Data (/Lab2)
    //-------------------------------------------------------------------------
    bool uploadLabData(
        const SensorData& sensorData,
        const ThreatData& threatData,
        const AlertData& alertData,
        const SystemStatus& systemStatus,
        const TimeManager& timeManager,
        uint32_t syncCount = 1
    );

    //-------------------------------------------------------------------------
    // Upload sensor data
    //-------------------------------------------------------------------------
    bool uploadSensorData(const SensorData& sensorData);

    //-------------------------------------------------------------------------
    // Upload threat information
    //-------------------------------------------------------------------------
    bool uploadThreatData(const ThreatData& threatData);

    //-------------------------------------------------------------------------
    // Upload alert information
    //-------------------------------------------------------------------------
    bool uploadAlertData(const AlertData& alertData);

    //-------------------------------------------------------------------------
    // Returns current Firebase status
    //-------------------------------------------------------------------------
    FirebaseStatus getStatus() const;

    //-------------------------------------------------------------------------
    // Returns true if Firebase is connected
    //-------------------------------------------------------------------------
    bool isConnected() const;

    //-------------------------------------------------------------------------
    // Static Helper Conversion Methods
    //-------------------------------------------------------------------------
    static String getThreatLevelString(ThreatLevel level);
    static String getAlarmStateString(AlarmState state);
    static String getTimeModeString(TimeMode mode);

private:

    // Current Firebase status
    FirebaseStatus firebaseStatus;
};

#endif // FIREBASE_MANAGER_H