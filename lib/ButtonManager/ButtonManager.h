/******************************************************************************
 * SentinelAI-X
 * File        : ButtonManager.h
 * Description : Handles Security ARM/DISARM Button
 ******************************************************************************/

#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "common.h"

class ButtonManager
{
public:

    ButtonManager();

    void begin();

    void update(bool isThreatActive = false);

    bool isPressed();

    bool isArmed() const;

    bool isAlarmMuted() const;

    bool isManualPanicActive() const;

    void resetMute();

    void setArmed(bool state);

private:

    bool currentState;
    bool lastState;
    bool pressed;

    bool armed;
    bool alarmMuted;
    bool manualPanicTriggered;

    unsigned long lastDebounceTime;

    const unsigned long debounceDelay = 50;
};

#endif