#ifndef CONFIG_H
#define CONFIG_H

/******************************************************************************
 * SentinelAI-X
 * File        : config.h
 * Description : Global configuration settings
 ******************************************************************************/

#define ENABLE_TIME_TEST_PROMPT true


//==============================================================================
// GPIO PIN CONFIGURATION
//==============================================================================

constexpr unsigned long SLOW_BLINK_INTERVAL = 500;
constexpr unsigned long FAST_BLINK_INTERVAL = 150;


// PIR Motion Sensor

constexpr uint8_t PIR_PIN = 12;


// MQ-2 Gas Sensor

constexpr uint8_t MQ2_PIN = 34;


// DHT11 Sensor

constexpr uint8_t DHT_PIN = 4;



// LEDs

constexpr uint8_t GREEN_LED_PIN = 27;
constexpr uint8_t RED_LED_PIN   = 26;


// Buzzer

constexpr uint8_t BUZZER_PIN = 25;


// Push Button

constexpr uint8_t BUTTON_PIN = 33;



//==============================================================================
// DHT CONFIGURATION
//==============================================================================

#define DHT_TYPE DHT11



//==============================================================================
// MQ-2 GAS THRESHOLDS
//==============================================================================

constexpr uint16_t MQ2_LOW_THRESHOLD      = 1500;

constexpr uint16_t MQ2_MEDIUM_THRESHOLD   = 2000;

constexpr uint16_t MQ2_HIGH_THRESHOLD     = 3000;



//==============================================================================
// TEMPERATURE THRESHOLDS
//==============================================================================

constexpr float TEMP_HIGH_THRESHOLD = 40.0f;

constexpr float TEMP_CRITICAL_THRESHOLD = 50.0f;



//==============================================================================
// DAY / NIGHT SETTINGS
//==============================================================================

constexpr uint8_t DAY_START_HOUR   = 6;

constexpr uint8_t NIGHT_START_HOUR = 18;



//==============================================================================
// NTP TIME CONFIGURATION
//==============================================================================

// India Standard Time (IST)
// UTC + 5:30

constexpr long GMT_OFFSET_SECONDS = 19800;


// India has no daylight saving

constexpr int DAYLIGHT_OFFSET_SECONDS = 0;



// NTP Servers

#define NTP_SERVER_1 "pool.ntp.org"

#define NTP_SERVER_2 "time.nist.gov"



//==============================================================================
// SENSOR UPDATE INTERVALS
//==============================================================================

constexpr unsigned long PIR_READ_INTERVAL = 200;

constexpr unsigned long MQ2_READ_INTERVAL = 1000;

constexpr unsigned long DHT_READ_INTERVAL = 2000;

constexpr unsigned long STATUS_SEND_INTERVAL = 60000;

constexpr unsigned long SERIAL_MONITOR_INTERVAL = 1000;

constexpr unsigned long FIREBASE_SENSOR_INTERVAL = 1000;



//==============================================================================
// BUZZER TIMING
//==============================================================================

constexpr uint16_t SHORT_BEEP_TIME = 300;

constexpr uint16_t BEEP_GAP_TIME = 200;

constexpr uint16_t FAST_BEEP_TIME = 300;



#endif // CONFIG_H