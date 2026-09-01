/******************************************************************************
 * SentinelAI-X
 * File        : LEDManager.cpp
 * Description : LED Control Module
 ******************************************************************************/

#include "LEDManager.h"


// Change pins according to your wiring

#define GREEN_LED_PIN 27
#define RED_LED_PIN   26



// Constructor

LEDManager::LEDManager()
{

    armedStatus = false;

    alertStatus = false;

}



// Initialize

void LEDManager::begin()
{

    pinMode(GREEN_LED_PIN, OUTPUT);

    pinMode(RED_LED_PIN, OUTPUT);



    digitalWrite(GREEN_LED_PIN, HIGH);

    digitalWrite(RED_LED_PIN, LOW);

}



// Security mode control

void LEDManager::setSecurityStatus(bool armed)
{

    armedStatus = armed;


    updateLEDs();

}



// Alert control

void LEDManager::setAlert(bool alert)
{

    alertStatus = alert;


    updateLEDs();

}



// Update LEDs

void LEDManager::updateLEDs()
{


    if(alertStatus)
    {

        // Danger condition

        digitalWrite(GREEN_LED_PIN, LOW);

        digitalWrite(RED_LED_PIN, HIGH);

    }


    else if(armedStatus)
    {

        // Security ON

        digitalWrite(GREEN_LED_PIN, LOW);

        digitalWrite(RED_LED_PIN, HIGH);

    }


    else
    {

        // Security OFF / Safe

        digitalWrite(GREEN_LED_PIN, HIGH);

        digitalWrite(RED_LED_PIN, LOW);

    }


}