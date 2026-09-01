#ifndef MQ2_MANAGER_H
#define MQ2_MANAGER_H

/******************************************************************************
 * SentinelAI-X
 * File        : MQ2Manager.h
 * Module      : MQ-2 Gas Sensor Manager
 * Description : Handles MQ-2 gas sensor reading and gas level classification.
 ******************************************************************************/

#include "common.h"


class MQ2Manager
{

public:

    // Constructor
    MQ2Manager();


    // Initialize MQ-2 sensor
    void begin();


    // Read MQ-2 sensor and update status
    void update();


    // Return gas status
    GasStatus getGasStatus() const;


    // Return raw ADC value
    int getRawValue() const;


private:

    // Raw ADC value from MQ-2
    int gasADC;


    // Current gas condition
    GasStatus gasStatus;

};


#endif // MQ2_MANAGER_H