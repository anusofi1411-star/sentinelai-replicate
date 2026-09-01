/******************************************************************************
 * SentinelAI-X
 * File        : LEDManager.h
 * Description : Controls Green and Red LEDs
 ******************************************************************************/

#ifndef LED_MANAGER_H
#define LED_MANAGER_H


#include "common.h"


class LEDManager
{

public:


    // Constructor
    LEDManager();



    // Initialize LEDs
    void begin();



    // System state LED control
    void setSecurityStatus(bool armed);



    // Alert control
    void setAlert(bool alert);



private:


    bool armedStatus;

    bool alertStatus;


    void updateLEDs();

};


#endif