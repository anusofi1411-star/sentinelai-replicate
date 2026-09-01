/******************************************************************************
 * SentinelAI-X
 * File        : AlarmManager.cpp
 * Description : Non-blocking controls for Green/Red LEDs and Buzzer based on Threat Level
 ******************************************************************************/

#include "AlarmManager.h"

//==============================================================================
// Constructor
//==============================================================================

AlarmManager::AlarmManager()
{
    currentAlarmState = AlarmState::ALARM_OFF;
    alarmAcknowledged = false;
    redLEDState = false;
    previousBlinkTime = 0;
    previousBeepTime = 0;
    patternStartMillis = 0;
    lastAlertActive = false;
}

//==============================================================================
// Initialize Hardware
//==============================================================================

void AlarmManager::begin()
{
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    // Initial state: SAFE (Green ON, Red OFF, Buzzer OFF)
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    patternStartMillis = millis();
}

//==============================================================================
// Update Alarm (100% Non-blocking)
//==============================================================================

void AlarmManager::update(const ThreatData& threatData)
{
    currentAlarmState = threatData.alarmState;
    unsigned long now = millis();

    bool isAlertActive = (threatData.threatLevel != ThreatLevel::SAFE && threatData.alarmState != AlarmState::ALARM_OFF);

    if (isAlertActive != lastAlertActive)
    {
        lastAlertActive = isAlertActive;
        if (isAlertActive)
        {
            Serial.println("\n[LED] RED -> BLINKING");
            Serial.println("[LED] GREEN -> OFF");
            Serial.println("[Buzzer] ALERT -> ON");
            Serial.println("[Buzzer] Mode -> ALERT");
        }
        else
        {
            Serial.println("\n[LED] RED -> OFF");
            Serial.println("[LED] GREEN -> ON");
            Serial.println("[Buzzer] ALERT -> OFF");
            Serial.println("[Buzzer] Mode -> NORMAL");
        }
    }

    if (threatData.threatLevel == ThreatLevel::SAFE)
    {
        safeMode();
        return;
    }

    // Threat is active -> Green LED must be OFF
    digitalWrite(GREEN_LED_PIN, LOW);

    // Synchronize Red LED and Buzzer according to alarmState
    switch (threatData.alarmState)
    {
        case AlarmState::ALARM_OFF:
        {
            // Muted or Alarm OFF during threat
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, LOW);
            break;
        }

        case AlarmState::SHORT_BEEP:
        {
            // Cycle: 2000 ms (100 ms ON, 1900 ms OFF)
            unsigned long cycle = (now - patternStartMillis) % 2000;
            bool active = (cycle < 100);
            digitalWrite(RED_LED_PIN, active ? HIGH : LOW);
            digitalWrite(BUZZER_PIN, active ? HIGH : LOW);
            break;
        }

        case AlarmState::DOUBLE_BEEP:
        {
            // Cycle: 2000 ms (100 ms ON, 150 ms OFF, 100 ms ON, 1650 ms OFF)
            unsigned long cycle = (now - patternStartMillis) % 2000;
            bool active = (cycle < 100) || (cycle >= 250 && cycle < 350);
            digitalWrite(RED_LED_PIN, active ? HIGH : LOW);
            digitalWrite(BUZZER_PIN, active ? HIGH : LOW);
            break;
        }

        case AlarmState::FAST_BEEP:
        {
            // Cycle: 400 ms (200 ms ON, 200 ms OFF)
            unsigned long cycle = (now - patternStartMillis) % 400;
            bool active = (cycle < 200);
            digitalWrite(RED_LED_PIN, active ? HIGH : LOW);
            digitalWrite(BUZZER_PIN, active ? HIGH : LOW);
            break;
        }

        case AlarmState::CONTINUOUS:
        {
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            break;
        }

        default:
        {
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(BUZZER_PIN, LOW);
            break;
        }
    }
}

//==============================================================================
// SAFE Mode
//==============================================================================

void AlarmManager::safeMode()
{
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
}

void AlarmManager::suspiciousMode()  { }
void AlarmManager::warningMode()     { }
void AlarmManager::highRiskMode()    { }
void AlarmManager::criticalMode()    { }
void AlarmManager::emergencyMode()   { }
void AlarmManager::slowBlink()       { }
void AlarmManager::fastBlink()       { }
void AlarmManager::buzzerOff()       { digitalWrite(BUZZER_PIN, LOW); }
void AlarmManager::shortBeep()       { }
void AlarmManager::doubleBeep()      { }
void AlarmManager::fastBeep()        { }
void AlarmManager::continuousBeep()  { digitalWrite(BUZZER_PIN, HIGH); }

void AlarmManager::acknowledgeAlarm()
{
    alarmAcknowledged = true;
}

void AlarmManager::reset()
{
    alarmAcknowledged = false;
    currentAlarmState = AlarmState::ALARM_OFF;
    safeMode();
}

AlarmState AlarmManager::getAlarmState() const
{
    return currentAlarmState;
}