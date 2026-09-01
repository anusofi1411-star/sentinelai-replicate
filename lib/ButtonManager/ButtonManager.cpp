#include "ButtonManager.h"


ButtonManager::ButtonManager()
{
    currentState = HIGH;
    lastState = HIGH;
    pressed = false;
    armed = true; // Default ARMED on startup
    alarmMuted = false;
    manualPanicTriggered = false;
    lastDebounceTime = 0;
}

void ButtonManager::begin()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void ButtonManager::update(bool isThreatActive)
{
    bool reading = digitalRead(BUTTON_PIN);

    if (reading != lastState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (reading != currentState)
        {
            currentState = reading;

            if (currentState == LOW) // Active-LOW Button Pressed
            {
                pressed = true;
                manualPanicTriggered = !manualPanicTriggered; // Toggle manual panic alert
                alarmMuted = false;

                if (manualPanicTriggered)
                {
                    Serial.println("\n[BUTTON ACTION] -> Panic Button PRESSED! Alert ACTIVATED.");
                }
                else
                {
                    Serial.println("\n[BUTTON ACTION] -> Panic Button PRESSED! Alert CLEARED / NORMAL.");
                }
            }
        }
    }

    lastState = reading;
}

bool ButtonManager::isPressed()
{
    bool state = pressed;
    pressed = false;
    return state;
}

bool ButtonManager::isArmed() const
{
    return armed;
}

bool ButtonManager::isAlarmMuted() const
{
    return alarmMuted;
}

bool ButtonManager::isManualPanicActive() const
{
    return manualPanicTriggered;
}

void ButtonManager::resetMute()
{
    alarmMuted = false;
}

void ButtonManager::setArmed(bool state)
{
    armed = state;
}