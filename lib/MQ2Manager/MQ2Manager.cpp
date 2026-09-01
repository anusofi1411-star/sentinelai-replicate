/******************************************************************************
 * SentinelAI-X
 * File        : MQ2Manager.cpp
 * Module      : MQ-2 Gas Sensor Manager
 * Description : Reads MQ-2 analog value and determines gas level.
 ******************************************************************************/

#include "MQ2Manager.h"


//==============================================================================
// Constructor
//==============================================================================

MQ2Manager::MQ2Manager()
{
    gasADC = 0;

    gasStatus = GasStatus::GAS_LOW;
}


//==============================================================================
// Initialize MQ-2 Sensor
//==============================================================================

void MQ2Manager::begin()
{
    pinMode(MQ2_PIN, INPUT);

    gasADC = 0;

    gasStatus = GasStatus::GAS_LOW;
}


//==============================================================================
// Read MQ-2 Sensor
//==============================================================================

void MQ2Manager::update()
{
    // Read analog value
    gasADC = analogRead(MQ2_PIN);


    // Classify gas level

    if(gasADC < MQ2_LOW_THRESHOLD)
    {
        gasStatus = GasStatus::GAS_LOW;
    }


    else if(gasADC < MQ2_MEDIUM_THRESHOLD)
    {
        gasStatus = GasStatus::GAS_MEDIUM;
    }


    else if(gasADC < MQ2_HIGH_THRESHOLD)
    {
        gasStatus = GasStatus::GAS_HIGH;
    }


    else
    {
        gasStatus = GasStatus::GAS_CRITICAL;
    }
}


//==============================================================================
// Return Gas Status
//==============================================================================

GasStatus MQ2Manager::getGasStatus() const
{
    return gasStatus;
}


//==============================================================================
// Return Raw ADC Value
//==============================================================================

int MQ2Manager::getRawValue() const
{
    return gasADC;
}