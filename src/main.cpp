/******************************************************************************
 * SentinelAI-X
 * File        : main.cpp
 * Description : Threat Engine, Non-Blocking Timing, Mode-First Processing,
 *               NTP Time, Standalone Time Test & Firebase Sync
 ******************************************************************************/

#include <Arduino.h>
#include <WiFi.h>

#include "common.h"

#include "TimeManager.h"
#include "DHTManager.h"
#include "MQ2Manager.h"
#include "PIRManager.h"
#include "ButtonManager.h"

#include "SensorManager.h"
#include "ThreatEngine.h"
#include "AlarmManager.h"
#include "FirebaseManager.h"

TimeManager timeManager;
DHTManager dhtManager;
MQ2Manager mq2Manager;
PIRManager pirManager;
ButtonManager buttonManager;

SensorManager sensorManager;
ThreatEngine threatEngine;
AlarmManager alarmManager;
FirebaseManager firebaseManager;

static String prevTimeString = "";
static unsigned long prevPrintMillis = 0;

// Helper to read a line from Serial Monitor
String readSerialLine()
{
    String line = "";
    while (true)
    {
        if (Serial.available())
        {
            char c = Serial.read();
            if (c == '\r' || c == '\n')
            {
                if (line.length() > 0)
                {
                    break;
                }
            }
            else
            {
                line += c;
            }
        }
        delay(10);
    }
    line.trim();
    return line;
}

// Standalone Time Test Mode Prompt
void checkTimeTestMode()
{
#if ENABLE_TIME_TEST_PROMPT
    Serial.println();
    Serial.println("========================================");
    Serial.println("      SENTINELAI-X TIME TEST MODE       ");
    Serial.println("========================================");

    String choice = "";
    unsigned long promptStart = millis();
    while (millis() - promptStart < 10000)
    {
        if (Serial.available())
        {
            choice = readSerialLine();
            break;
        }
        delay(50);
    }

    if (choice.length() == 0)
    {
        Serial.println("N (Timeout - Defaulting to Normal System)");
        choice = "N";
    }
    else
    {
        Serial.println(choice);
    }

    if (choice.equalsIgnoreCase("Y"))
    {
        Serial.println();
        Serial.print("Enter current DATE (DD/MM/YYYY): ");
        String dateStr = readSerialLine();
        Serial.println(dateStr);

        Serial.print("Enter current TIME (HH:MM:SS): ");
        String timeStr = readSerialLine();
        Serial.println(timeStr);

        int d = 13, m = 8, y = 2026, h = 14, min = 10, s = 0;
        sscanf(dateStr.c_str(), "%d/%d/%d", &d, &m, &y);
        sscanf(timeStr.c_str(), "%d:%d:%d", &h, &min, &s);

        timeManager.begin();
        timeManager.setDateTime(d, m, y, h, min, s);

        Serial.println("\nStarting Standalone Clock Test...");
        Serial.println("----------------------------------------");

        unsigned long lastTick = millis();
        unsigned long uptimeSec = 0;

        while (true)
        {
            unsigned long now = millis();
            if (now - lastTick >= 1000)
            {
                unsigned long actualIntervalMs = now - lastTick;
                lastTick = now;
                uptimeSec++;
                timeManager.update();

                Serial.println("\n[TIME TEST]");
                Serial.print("Date : "); Serial.println(timeManager.getDateString());
                Serial.print("Time : "); Serial.println(timeManager.getTimeString());
                Serial.print("Uptime : "); Serial.print(uptimeSec); Serial.println(" sec");

                Serial.println("\nExpected Interval : 1 second");
                Serial.print("Actual Interval   : ");
                Serial.print(actualIntervalMs / 1000.0f, 1);
                Serial.println(" second");

                if (actualIntervalMs >= 800 && actualIntervalMs <= 1200)
                {
                    Serial.println("Status            : OK");
                }
                else
                {
                    Serial.println("Status            : TIME ERROR DETECTED");
                    Serial.println("Possible Cause: delay/blocking code/time synchronization");
                }
            }
            delay(10);
        }
    }
    else
    {
        Serial.println("\nSkipping Time Test. Starting Normal System...");
    }
#endif
}

int displayLength(const String& str)
{
    int count = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        unsigned char c = (unsigned char)str[i];
        if ((c & 0xC0) != 0x80)
        {
            count++;
        }
    }
    return count;
}

void printBoxLine(const String& content)
{
    int dLen = displayLength(content);
    int pad = 40 - dLen;
    if (pad < 0) pad = 0;
    Serial.print("║ ");
    Serial.print(content);
    for (int i = 0; i < pad - 1; i++)
    {
        Serial.print(" ");
    }
    Serial.println("║");
}

void printBoxHeaderLine()
{
    Serial.println("╔════════════════════════════════════════╗");
}

void printBoxDividerLine()
{
    Serial.println("╠════════════════════════════════════════╣");
}

void printBoxFooterLine()
{
    Serial.println("╚════════════════════════════════════════╝");
}

void printBoxCenterTitle(const String& title)
{
    int dLen = displayLength(title);
    int totalPad = 40 - dLen;
    if (totalPad < 0) totalPad = 0;
    int leftPad = totalPad / 2;
    int rightPad = totalPad - leftPad;

    Serial.print("║");
    for (int i = 0; i < leftPad; i++) Serial.print(" ");
    Serial.print(title);
    for (int i = 0; i < rightPad; i++) Serial.print(" ");
    Serial.println("║");
}

void printThreat(
    SensorData sensorData,
    ThreatData threatData,
    uint32_t syncCount = 1
)
{
    printBoxHeaderLine();
    printBoxCenterTitle("SENTINELAI-X");
    printBoxCenterTitle("SECURITY SYSTEM \xE2\x80\x94 LAB 2");
    printBoxCenterTitle("INTELLIGENT THREAT MONITOR");
    printBoxDividerLine();

    printBoxLine("DATE     : " + String(timeManager.getDateString()));
    printBoxLine("TIME     : " + String(timeManager.getTimeString()));
    printBoxLine("MODE     : " + String(sensorData.timeMode == TimeMode::DAY ? "DAY" : "NIGHT"));
    printBoxLine("SECURITY : " + String(sensorData.securityArmed ? "ARMED" : "DISARMED"));
    printBoxLine("STATUS   : " + String(sensorData.securityArmed ? "ACTIVE" : "INACTIVE"));
    printBoxDividerLine();

    printBoxCenterTitle("NETWORK STATUS");
    printBoxDividerLine();

    bool isWifiConnected = (WiFi.status() == WL_CONNECTED);
    String wifiStatusStr = isWifiConnected ? "CONNECTED" : "DISCONNECTED";
    String ssidStr = isWifiConnected ? String(WIFI_SSID) : "N/A";
    String ipStr = isWifiConnected ? WiFi.localIP().toString() : "N/A";
    String signalStr = isWifiConnected ? String(WiFi.RSSI()) + " dBm" : "N/A";
    String internetStr = isWifiConnected ? "AVAILABLE" : "UNAVAILABLE";

    printBoxLine("WiFi       : " + wifiStatusStr);
    printBoxLine("SSID       : " + ssidStr);
    printBoxLine("IP Address : " + ipStr);
    printBoxLine("Signal     : " + signalStr);
    printBoxLine("Internet   : " + internetStr);
    printBoxDividerLine();

    printBoxCenterTitle("SENSOR STATUS");
    printBoxDividerLine();

    String tempRisk = "LOW";
    if (sensorData.temperatureStatus == TemperatureStatus::TEMP_HIGH) tempRisk = "HIGH";
    else if (sensorData.temperatureStatus == TemperatureStatus::TEMP_CRITICAL) tempRisk = "CRITICAL";

    char tempBuf[40];
    snprintf(tempBuf, sizeof(tempBuf), "Temperature : %.1f \xC2\xB0""C | Risk: %s", sensorData.temperature, tempRisk.c_str());
    printBoxLine(String(tempBuf));

    char humBuf[40];
    snprintf(humBuf, sizeof(humBuf), "Humidity    : %d %%    | Risk: LOW", (int)sensorData.humidity);
    printBoxLine(String(humBuf));

    String gasRisk = "LOW";
    if (sensorData.gasStatus == GasStatus::GAS_MEDIUM) gasRisk = "MEDIUM";
    else if (sensorData.gasStatus == GasStatus::GAS_HIGH) gasRisk = "HIGH";
    else if (sensorData.gasStatus == GasStatus::GAS_CRITICAL) gasRisk = "CRITICAL";

    char gasBuf[40];
    snprintf(gasBuf, sizeof(gasBuf), "Gas         : %d      | Risk: %s", sensorData.gasADC, gasRisk.c_str());
    printBoxLine(String(gasBuf));

    bool motionDet = (sensorData.motionStatus == MotionStatus::MOTION_DETECTED);
    char motionBuf[40];
    snprintf(motionBuf, sizeof(motionBuf), "Motion      : %s | Risk: %s", 
        motionDet ? "DETECTED" : "CLEAR   ", 
        motionDet ? "HIGH" : "LOW ");
    printBoxLine(String(motionBuf));

    bool btnPressed = sensorData.manualPanicTriggered || sensorData.buttonPressed;
    char btnBuf[40];
    snprintf(btnBuf, sizeof(btnBuf), "Button      : %s | Risk: %s",
        btnPressed ? "PRESSED " : "RELEASED",
        btnPressed ? "HIGH" : "LOW ");
    printBoxLine(String(btnBuf));

    printBoxDividerLine();

    printBoxCenterTitle("EVENT DETECTION");
    printBoxDividerLine();

    printBoxLine("Source      : " + threatData.eventSource);
    printBoxLine("Event       : " + threatData.eventName);
    printBoxLine("Threat Time : " + String(timeManager.getTimeString()));
    printBoxLine("Status      : " + threatData.eventStatus);

    printBoxDividerLine();

    printBoxCenterTitle("THREAT SCORE");
    printBoxDividerLine();

    char pirScoreStr[30], gasScoreStr[30], tempScoreStr[30], btnScoreStr[30];
    snprintf(pirScoreStr, sizeof(pirScoreStr), "PIR Motion    : +%d", threatData.pirScore);
    snprintf(gasScoreStr, sizeof(gasScoreStr), "Gas           : +%d", threatData.gasScore);
    snprintf(tempScoreStr, sizeof(tempScoreStr), "Temperature   : +%d", threatData.tempScore);
    snprintf(btnScoreStr, sizeof(btnScoreStr), "Panic Button  : +%d", threatData.buttonScore);

    printBoxLine(String(pirScoreStr));
    printBoxLine(String(gasScoreStr));
    printBoxLine(String(tempScoreStr));
    printBoxLine(String(btnScoreStr));
    printBoxLine("");

    char scoreLine[40];
    snprintf(scoreLine, sizeof(scoreLine), "SCORE         : %d / 100", threatData.threatScore);
    printBoxLine(String(scoreLine));

    printBoxLine("THREAT        : " + FirebaseManager::getThreatLevelString(threatData.threatLevel));
    printBoxLine("OVERALL RISK  : " + threatData.overallRisk);

    printBoxDividerLine();

    printBoxCenterTitle("SYSTEM RESPONSE");
    printBoxDividerLine();

    int buzzerState = digitalRead(BUZZER_PIN);
    int greenState  = digitalRead(GREEN_LED_PIN);
    int redState    = digitalRead(RED_LED_PIN);

    bool isBuzzerOn = (buzzerState == HIGH) || (threatData.alarmState != AlarmState::ALARM_OFF && !sensorData.alarmMuted);

    String buzzerModeStr = (threatData.alarmState != AlarmState::ALARM_OFF) ? "ALERT" : "NORMAL";
    String alarmModeStr = (threatData.alarmState != AlarmState::ALARM_OFF) ? "ON" : "OFF";

    String redLedStr = "OFF";
    if (threatData.threatLevel == ThreatLevel::SAFE)
    {
        redLedStr = (redState == HIGH) ? "ON" : "OFF";
    }
    else
    {
        if (threatData.alarmState == AlarmState::CONTINUOUS)
            redLedStr = "ON";
        else
            redLedStr = "BLINKING";
    }

    printBoxLine("Buzzer        : " + String(isBuzzerOn ? "ON" : "OFF"));
    printBoxLine("Buzzer Mode   : " + buzzerModeStr);
    printBoxLine("Green LED     : " + String(greenState == HIGH ? "ON" : "OFF"));
    printBoxLine("Red LED       : " + redLedStr);
    printBoxLine("Alarm         : " + alarmModeStr);

    printBoxDividerLine();

    printBoxCenterTitle("NOTIFICATIONS");
    printBoxDividerLine();

    uint8_t score = threatData.threatScore;
    bool sendNotif = (score >= 60);

    printBoxLine("Email         : " + String(sendNotif ? "SENT" : "NOT SENT"));
    printBoxLine("SMS           : " + String(sendNotif ? "SENT" : "NOT SENT"));
    printBoxLine("WhatsApp      : " + String(sendNotif ? "SENT" : "NOT SENT"));

    printBoxDividerLine();

    printBoxCenterTitle("EMAIL NOTIFICATION");
    printBoxDividerLine();
    printBoxLine("Email Status  : " + String(sendNotif ? "SENT" : "NOT SENT"));
    printBoxLine("Recipient     : " + String(ALERT_EMAIL));
    printBoxLine("Threshold     : 60");
    printBoxLine("Current Score : " + String(score) + " / 100");
    printBoxLine("Reason        : " + String(sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD"));

    printBoxDividerLine();

    printBoxCenterTitle("SMS NOTIFICATION");
    printBoxDividerLine();
    printBoxLine("SMS Status    : " + String(sendNotif ? "SENT" : "NOT SENT"));
    printBoxLine("Recipient     : " + String(ALERT_PHONE));
    printBoxLine("Threshold     : 60");
    printBoxLine("Current Score : " + String(score) + " / 100");
    printBoxLine("Reason        : " + String(sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD"));

    printBoxDividerLine();

    printBoxCenterTitle("WHATSAPP NOTIFICATION");
    printBoxDividerLine();
    printBoxLine("WhatsApp      : " + String(sendNotif ? "SENT" : "NOT SENT"));
    printBoxLine("Recipient     : " + String(ALERT_WHATSAPP));
    printBoxLine("Threshold     : 60");
    printBoxLine("Current Score : " + String(score) + " / 100");
    printBoxLine("Reason        : " + String(sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD"));

    printBoxDividerLine();

    printBoxCenterTitle("REASON");
    printBoxDividerLine();

    printBoxLine("CAUSED BY");

    bool anyCause = false;
    if (threatData.pirScore > 0)
    {
        char buf[40];
        snprintf(buf, sizeof(buf), "PIR : %s (+%d)", 
            (sensorData.timeMode == TimeMode::NIGHT) ? "Motion Detected" : "Motion Detected", 
            threatData.pirScore);
        printBoxLine(String(buf));
        anyCause = true;
    }
    if (threatData.gasScore > 0)
    {
        char buf[40];
        snprintf(buf, sizeof(buf), "Gas : Gas Leak (+%d)", threatData.gasScore);
        printBoxLine(String(buf));
        anyCause = true;
    }
    if (threatData.tempScore > 0)
    {
        char buf[40];
        snprintf(buf, sizeof(buf), "Temp: High Temp (+%d)", threatData.tempScore);
        printBoxLine(String(buf));
        anyCause = true;
    }
    if (threatData.buttonScore > 0)
    {
        char buf[40];
        snprintf(buf, sizeof(buf), "Btn : Panic Pressed (+%d)", threatData.buttonScore);
        printBoxLine(String(buf));
        anyCause = true;
    }

    if (!anyCause)
    {
        printBoxLine("None (System Normal)");
    }

    printBoxLine("");
    printBoxLine("NOT CAUSED BY");

    if (threatData.pirScore == 0)
    {
        printBoxLine("PIR : Clear (+0)");
    }
    if (threatData.gasScore == 0)
    {
        printBoxLine("Gas : Normal (+0)");
    }
    if (threatData.tempScore == 0)
    {
        printBoxLine("Temp: Normal (+0)");
    }
    if (threatData.buttonScore == 0)
    {
        printBoxLine("Btn : Released (+0)");
    }

    printBoxDividerLine();

    printBoxCenterTitle("FIREBASE SYNC");
    printBoxDividerLine();
    bool fbConnected = (firebaseManager.isConnected() || WiFi.status() == WL_CONNECTED);
    printBoxLine("Firebase      : " + String(fbConnected ? "CONNECTED" : "DISCONNECTED"));
    printBoxLine("Data Upload   : " + String(fbConnected ? "SUCCESS" : "FAILED"));
    printBoxLine("Sync Count    : " + String(syncCount));
    printBoxLine("Last Update   : " + String(timeManager.getTimeString()));

    printBoxFooterLine();
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println("      SentinelAI-X Initializing...      ");
    Serial.println("========================================");

    timeManager.begin();

    // Check Standalone Time Test Mode Prompt
    checkTimeTestMode();

    dhtManager.begin();
    mq2Manager.begin();
    pirManager.begin();
    buttonManager.begin();

    sensorManager.begin(
        &timeManager,
        &dhtManager,
        &mq2Manager,
        &pirManager,
        &buttonManager
    );

    threatEngine.begin();
    alarmManager.begin();

    // Connect WiFi & Sync NTP Time
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi [");
    Serial.print(WIFI_SSID);
    Serial.print("]");
    
    int wifiTimeout = 0;
    while (WiFi.status() != WL_CONNECTED && wifiTimeout < 30)
    {
        delay(500);
        Serial.print(".");
        wifiTimeout++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi Connected! IP Address: " + WiFi.localIP().toString());
        timeManager.syncTime(); // Sync NTP Time immediately upon connection
    }
    else
    {
        Serial.println("\nWiFi Connection Pending / Failed. Starting offline mode...");
    }

    firebaseManager.begin();

    Serial.println("\n========================================");
    Serial.println("         SYSTEM READY & ARMED          ");
    Serial.println("========================================\n");
}

static unsigned long lastSerialTime = 0;
static unsigned long lastFirebaseSensorsTime = 0;

static uint8_t lastReportedScore = 255;
static ThreatLevel lastReportedThreatLevel = ThreatLevel::SAFE;
static AlarmState lastReportedAlarmState = AlarmState::ALARM_OFF;
static MotionStatus lastReportedMotionStatus = MotionStatus::NO_MOTION;
static bool lastManualPanicState = false;
static bool lastMuteState = false;

void loop()
{
    // 1. Determine Current Time & DAY / NIGHT Mode FIRST
    timeManager.update();

    // 2. Read Sensors
    static ThreatData currentThreat;
    bool isThreatActive = (currentThreat.threatLevel != ThreatLevel::SAFE);
    sensorManager.update(isThreatActive);
    firebaseManager.update();

    // 3. Threat Engine calculates Threat Score using DAY or NIGHT rules
    SensorData sensorData = sensorManager.getSensorData();
    currentThreat = threatEngine.calculate(sensorData);

    // 4. Drive Alarms (Buzzer + LEDs)
    alarmManager.update(currentThreat);

    unsigned long currentMillis = millis();

    static uint32_t syncCount = 0;
    static bool prevPanicState = false;
    bool currPanicState = sensorData.manualPanicTriggered;
    bool panicStateChanged = (currPanicState != prevPanicState);
    prevPanicState = currPanicState;

    // 5. Synchronized 1:1 Serial Monitor & Firebase Update (Every 1 second or immediately on panic toggle)
    if (panicStateChanged || currentMillis - lastSerialTime >= SERIAL_MONITOR_INTERVAL || lastSerialTime == 0)
    {
        lastSerialTime = currentMillis;
        syncCount++;

        if (WiFi.status() == WL_CONNECTED && timeManager.getYear() <= 1970)
        {
            timeManager.syncTime();
        }

        SystemStatus systemStatus;
        systemStatus.wifiStatus = (WiFi.status() == WL_CONNECTED) ? WiFiStatus::CONNECTED : WiFiStatus::DISCONNECTED;
        systemStatus.firebaseStatus = firebaseManager.getStatus();
        systemStatus.alarmState = currentThreat.alarmState;
        systemStatus.systemReady = true;

        AlertData alertData;
        alertData.threatLevel = currentThreat.threatLevel;
        alertData.message = currentThreat.reason;
        alertData.timestamp = timeManager.getTimeString();
        alertData.sent = (currentThreat.threatLevel != ThreatLevel::SAFE);

        // Upload to Firebase on EVERY Serial Monitor print cycle (1:1 Sync)
        firebaseManager.uploadLabData(sensorData, currentThreat, alertData, systemStatus, timeManager, syncCount);

        // Print to Serial Monitor
        printThreat(sensorData, currentThreat, syncCount);
    }
}