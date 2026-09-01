#ifndef ENUMS_H
#define ENUMS_H

/******************************************************************************
 * SentinelAI-X
 * File        : enums.h
 * Description : All common enumerations used across the project.
 ******************************************************************************/


// Time mode
enum class TimeMode
{
    DAY = 0,
    NIGHT
};


// Gas sensor status
// LOW, HIGH names avoided because Arduino already uses them
enum class GasStatus
{
    GAS_LOW = 0,
    GAS_MEDIUM,
    GAS_HIGH,
    GAS_CRITICAL
};


// Temperature status
enum class TemperatureStatus
{
    TEMP_NORMAL = 0,
    TEMP_HIGH,
    TEMP_CRITICAL
};


// Motion status
enum class MotionStatus
{
    NO_MOTION = 0,
    MOTION_DETECTED
};


// Threat level
enum class ThreatLevel
{
    SAFE = 0,
    SUSPICIOUS,
    WARNING,
    HIGH_RISK,
    CRITICAL,
    EMERGENCY
};


// Alarm state
// OFF name avoided to prevent conflicts
enum class AlarmState
{
    ALARM_OFF = 0,
    SHORT_BEEP,
    DOUBLE_BEEP,
    FAST_BEEP,
    CONTINUOUS
};


// Wi-Fi status
enum class WiFiStatus
{
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED
};


// Firebase status
enum class FirebaseStatus
{
    FIREBASE_DISCONNECTED = 0,
    FIREBASE_CONNECTED,
    SYNCING,
    FIREBASE_ERROR
};


#endif // ENUMS_H