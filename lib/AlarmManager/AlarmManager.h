#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : AlarmManager.h
 * Description : Controls LEDs and Buzzer based on Threat Level
 ******************************************************************************/

#include <common.h>

class AlarmManager
{
public:

    //==========================================================================
    // Constructor
    //==========================================================================

    AlarmManager();

    //==========================================================================
    // Initialize Alarm Hardware
    //==========================================================================

    void begin();

    //==========================================================================
    // Update Alarm System
    //==========================================================================

    void update(const ThreatData& threatData);

    //==========================================================================
    // Alarm Acknowledgement
    //==========================================================================

    void acknowledgeAlarm();

    //==========================================================================
    // Reset Alarm
    //==========================================================================

    void reset();

    //==========================================================================
    // Get Current Alarm State
    //==========================================================================

    AlarmState getAlarmState() const;

private:

    //==========================================================================
    // LED Control
    //==========================================================================

    void safeMode();

    void suspiciousMode();

    void warningMode();

    void highRiskMode();

    void criticalMode();

    void emergencyMode();

    //==========================================================================
    // Red LED Blink
    //==========================================================================

    void slowBlink();

    void fastBlink();

    //==========================================================================
    // Buzzer Control
    //==========================================================================

    void buzzerOff();

    void shortBeep();

    void doubleBeep();

    void fastBeep();

    void continuousBeep();

    //==========================================================================
    // Variables
    //==========================================================================

    AlarmState currentAlarmState;

    bool alarmAcknowledged;

    bool redLEDState;

    unsigned long previousBlinkTime;

    unsigned long previousBeepTime;

    unsigned long patternStartMillis;

    bool lastAlertActive;
};

#endif // ALARM_MANAGER_H