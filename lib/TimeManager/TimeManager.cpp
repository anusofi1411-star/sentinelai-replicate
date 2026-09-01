/******************************************************************************
 * SentinelAI-X
 * File        : TimeManager.cpp
 * Description : NTP Time Manager
 ******************************************************************************/

#include "TimeManager.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define GMT_OFFSET       19800
#define DAYLIGHT_OFFSET  0

//==============================================================================
// Constructor
//==============================================================================

TimeManager::TimeManager()
{
    hour = 0;
    minute = 0;
    second = 0;

    day = 1;
    month = 1;
    year = 2026;

    strcpy(weekdayName, "Unknown");

    mode = TimeMode::DAY;
}

//==============================================================================
// Initialize
//==============================================================================

void TimeManager::begin()
{
    hour = 0;
    minute = 0;
    second = 0;

    day = 1;
    month = 1;
    year = 2026;

    strcpy(weekdayName, "Unknown");

    mode = TimeMode::DAY;
}

//==============================================================================
// Synchronize NTP Time
//==============================================================================

void TimeManager::syncTime()
{
    Serial.println();
    Serial.println("Synchronizing Time With NTP...");

    configTime(
        GMT_OFFSET,
        DAYLIGHT_OFFSET,
        "pool.ntp.org",
        "time.nist.gov"
    );

    struct tm timeInfo;

    int retry = 0;

    while (!getLocalTime(&timeInfo, 200) && retry < 20)
    {
        Serial.print(".");
        delay(250);
        retry++;
    }

    if (retry >= 20)
    {
        Serial.println();
        Serial.println("NTP Synchronization Failed.");
        return;
    }

    Serial.println();
    Serial.println("NTP Synchronization Successful.");

    update();
}

//==============================================================================
// Manually Set Date & Time
//==============================================================================

void TimeManager::setDateTime(uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min, uint8_t s)
{
    day = d;
    month = m;
    year = y;
    hour = h;
    minute = min;
    second = s;

    struct tm timeInfo;
    memset(&timeInfo, 0, sizeof(timeInfo));
    timeInfo.tm_mday = d;
    timeInfo.tm_mon = m - 1;
    timeInfo.tm_year = y - 1900;
    timeInfo.tm_hour = h;
    timeInfo.tm_min = min;
    timeInfo.tm_sec = s;

    time_t t = mktime(&timeInfo);
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    update();
}

//==============================================================================
// Update Current Time
//==============================================================================

void TimeManager::update()
{
    struct tm timeInfo;

    // Use non-blocking timeout (10 ms) so loop() is never delayed
    if (getLocalTime(&timeInfo, 10))
    {
        // Time
        hour   = timeInfo.tm_hour;
        minute = timeInfo.tm_min;
        second = timeInfo.tm_sec;

        // Date
        day    = timeInfo.tm_mday;
        month  = timeInfo.tm_mon + 1;
        year   = timeInfo.tm_year + 1900;

        // Weekday
        strcpy(weekdayName, getWeekdayName(timeInfo.tm_wday));
    }

    // Day / Night Detection
    if (hour >= 6 && hour < 18)
    {
        mode = TimeMode::DAY;
    }
    else
    {
        mode = TimeMode::NIGHT;
    }
}




//==============================================================================
// Convert Weekday Number To Name
//==============================================================================

const char* TimeManager::getWeekdayName(int weekday)
{
    switch (weekday)
    {
        case 0:
            return "Sunday";

        case 1:
            return "Monday";

        case 2:
            return "Tuesday";

        case 3:
            return "Wednesday";

        case 4:
            return "Thursday";

        case 5:
            return "Friday";

        case 6:
            return "Saturday";

        default:
            return "Unknown";
    }
}
//==============================================================================
// Get Time Mode
//==============================================================================

TimeMode TimeManager::getTimeMode() const
{
    return mode;
}


//==============================================================================
// Get Time Values
//==============================================================================

uint8_t TimeManager::getHour() const
{
    return hour;
}

uint8_t TimeManager::getMinute() const
{
    return minute;
}

uint8_t TimeManager::getSecond() const
{
    return second;
}


//==============================================================================
// Get Date Values
//==============================================================================

uint8_t TimeManager::getDay() const
{
    return day;
}

uint8_t TimeManager::getMonth() const
{
    return month;
}

uint16_t TimeManager::getYear() const
{
    return year;
}


//==============================================================================
// Get Weekday
//==============================================================================

const char* TimeManager::getWeekday() const
{
    return weekdayName;
}


//==============================================================================
// Get Formatted Date
//==============================================================================

const char* TimeManager::getDateString() const
{
    snprintf(
        dateBuffer,
        sizeof(dateBuffer),
        "%02d/%02d/%04d",
        day,
        month,
        year
    );

    return dateBuffer;
}


//==============================================================================
// Get Formatted Time
//==============================================================================

const char* TimeManager::getTimeString() const
{
    snprintf(
        timeBuffer,
        sizeof(timeBuffer),
        "%02d:%02d:%02d",
        hour,
        minute,
        second
    );

    return timeBuffer;
}