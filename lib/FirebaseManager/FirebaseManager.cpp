/******************************************************************************
 * SentinelAI-X
 * File        : FirebaseManager.cpp
 * Module      : Firebase Manager
 * Description : Handles Firebase RTDB publishing under /Lab2 node.
 ******************************************************************************/

#include "FirebaseManager.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include <FirebaseESP32.h>

static FirebaseData fbdo;
static FirebaseAuth auth;
static FirebaseConfig config;

FirebaseManager::FirebaseManager()
{
    firebaseStatus = FirebaseStatus::FIREBASE_DISCONNECTED;
}

void FirebaseManager::begin()
{
    connect();
}

bool FirebaseManager::connect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        firebaseStatus = FirebaseStatus::FIREBASE_DISCONNECTED;
        return false;
    }

    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_API_KEY;

    // Buffer & timeout configurations to prevent SSL buffer size overflow & connection drops
    config.timeout.serverResponse = 10000;

    fbdo.setResponseSize(4096);
    fbdo.setBSSLBufferSize(4096, 1024);

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    firebaseStatus = FirebaseStatus::FIREBASE_CONNECTED;
    return true;
}

void FirebaseManager::update()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (firebaseStatus == FirebaseStatus::FIREBASE_DISCONNECTED)
        {
            connect();
        }
    }
    else
    {
        firebaseStatus = FirebaseStatus::FIREBASE_DISCONNECTED;
    }
}

String FirebaseManager::getThreatLevelString(ThreatLevel level)
{
    switch (level)
    {
        case ThreatLevel::SAFE:        return "SAFE";
        case ThreatLevel::SUSPICIOUS:  return "SUSPICIOUS";
        case ThreatLevel::WARNING:     return "WARNING";
        case ThreatLevel::HIGH_RISK:   return "HIGH_RISK";
        case ThreatLevel::CRITICAL:    return "CRITICAL";
        case ThreatLevel::EMERGENCY:   return "EMERGENCY";
        default:                       return "SAFE";
    }
}

String FirebaseManager::getAlarmStateString(AlarmState state)
{
    switch (state)
    {
        case AlarmState::ALARM_OFF:    return "ALARM_OFF";
        case AlarmState::SHORT_BEEP:   return "SHORT_BEEP";
        case AlarmState::DOUBLE_BEEP:  return "DOUBLE_BEEP";
        case AlarmState::FAST_BEEP:    return "FAST_BEEP";
        case AlarmState::CONTINUOUS:   return "CONTINUOUS";
        default:                       return "ALARM_OFF";
    }
}

String FirebaseManager::getTimeModeString(TimeMode mode)
{
    return (mode == TimeMode::DAY) ? "DAY" : "NIGHT";
}

bool FirebaseManager::uploadLabData(
    const SensorData& sensorData,
    const ThreatData& threatData,
    const AlertData& alertData,
    const SystemStatus& systemStatus,
    const TimeManager& timeManager,
    uint32_t syncCount
)
{
    String dateStr = timeManager.getDateString();
    String timeStr = timeManager.getTimeString();

    JsonDocument doc;

    // 1. system
    JsonObject systemObj = doc["system"].to<JsonObject>();
    systemObj["date"] = dateStr;
    systemObj["time"] = timeStr;
    systemObj["mode"] = (sensorData.timeMode == TimeMode::DAY) ? "DAY" : "NIGHT";
    systemObj["security"] = sensorData.securityArmed ? "ARMED" : "DISARMED";
    systemObj["status"] = sensorData.securityArmed ? "ACTIVE" : "INACTIVE";

    // 2. network
    JsonObject networkObj = doc["network"].to<JsonObject>();
    JsonObject wifiObj = networkObj["wifi"].to<JsonObject>();
    bool isConnected = (WiFi.status() == WL_CONNECTED);
    wifiObj["status"] = isConnected ? "CONNECTED" : "DISCONNECTED";
    wifiObj["ssid"] = isConnected ? WIFI_SSID : "N/A";
    wifiObj["ip_address"] = isConnected ? WiFi.localIP().toString() : "N/A";
    wifiObj["signal_strength"] = isConnected ? WiFi.RSSI() : 0;
    wifiObj["signal_unit"] = "dBm";
    networkObj["internet"] = isConnected ? "AVAILABLE" : "UNAVAILABLE";

    // 2. sensors
    JsonObject sensorsObj = doc["sensors"].to<JsonObject>();

    // temperature
    JsonObject tempObj = sensorsObj["temperature"].to<JsonObject>();
    tempObj["value"] = sensorData.temperature;
    tempObj["unit"] = "\xC2\xB0""C";
    String tempRisk = "LOW";
    if (sensorData.temperatureStatus == TemperatureStatus::TEMP_HIGH) tempRisk = "HIGH";
    else if (sensorData.temperatureStatus == TemperatureStatus::TEMP_CRITICAL) tempRisk = "CRITICAL";
    tempObj["risk"] = tempRisk;

    // humidity
    JsonObject humObj = sensorsObj["humidity"].to<JsonObject>();
    humObj["value"] = (int)sensorData.humidity;
    humObj["unit"] = "%";
    humObj["risk"] = "LOW";

    // gas
    JsonObject gasObj = sensorsObj["gas"].to<JsonObject>();
    gasObj["value"] = sensorData.gasADC;
    String gasRisk = "LOW";
    if (sensorData.gasStatus == GasStatus::GAS_MEDIUM) gasRisk = "MEDIUM";
    else if (sensorData.gasStatus == GasStatus::GAS_HIGH) gasRisk = "HIGH";
    else if (sensorData.gasStatus == GasStatus::GAS_CRITICAL) gasRisk = "CRITICAL";
    gasObj["risk"] = gasRisk;

    // motion
    JsonObject motionObj = sensorsObj["motion"].to<JsonObject>();
    bool hasMotion = (sensorData.motionStatus == MotionStatus::MOTION_DETECTED);
    motionObj["value"] = hasMotion ? "DETECTED" : "CLEAR";
    motionObj["risk"] = hasMotion ? "HIGH" : "LOW";

    // button
    JsonObject btnObj = sensorsObj["button"].to<JsonObject>();
    bool btnPressed = sensorData.manualPanicTriggered || sensorData.buttonPressed;
    btnObj["value"] = btnPressed ? "PRESSED" : "RELEASED";
    btnObj["risk"] = btnPressed ? "HIGH" : "LOW";

    // 3. threat
    JsonObject threatObj = doc["threat"].to<JsonObject>();
    threatObj["score"] = threatData.threatScore;
    threatObj["max_score"] = 100;
    threatObj["level"] = getThreatLevelString(threatData.threatLevel);
    threatObj["overall_risk"] = threatData.overallRisk;
    threatObj["reason"] = threatData.reason;

    // 4. events
    JsonObject eventsObj = doc["events"].to<JsonObject>();
    eventsObj["source"] = threatData.eventSource;
    eventsObj["event"] = threatData.eventName;
    eventsObj["time"] = timeStr;
    eventsObj["status"] = threatData.eventStatus;

    // 5. score_breakdown
    JsonObject breakdownObj = doc["score_breakdown"].to<JsonObject>();
    breakdownObj["pir"] = threatData.pirScore;
    breakdownObj["gas"] = threatData.gasScore;
    breakdownObj["temperature"] = threatData.tempScore;
    breakdownObj["panic_button"] = threatData.buttonScore;

    // 6. response
    JsonObject responseObj = doc["response"].to<JsonObject>();

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

    responseObj["buzzer"] = isBuzzerOn ? "ON" : "OFF";
    responseObj["buzzer_mode"] = buzzerModeStr;
    responseObj["green_led"] = (greenState == HIGH) ? "ON" : "OFF";
    responseObj["red_led"] = redLedStr;
    responseObj["alarm"] = alarmModeStr;

    // 7. notifications
    JsonObject notifObj = doc["notifications"].to<JsonObject>();

    uint8_t score = threatData.threatScore;
    bool sendNotif = (score >= 60);

    JsonObject emailObj = notifObj["email"].to<JsonObject>();
    emailObj["status"] = sendNotif ? "SENT" : "NOT SENT";
    emailObj["recipient"] = ALERT_EMAIL;
    emailObj["threshold"] = 60;
    emailObj["current_score"] = score;
    emailObj["reason"] = sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD";

    JsonObject smsObj = notifObj["sms"].to<JsonObject>();
    smsObj["status"] = sendNotif ? "SENT" : "NOT SENT";
    smsObj["recipient"] = ALERT_PHONE;
    smsObj["threshold"] = 60;
    smsObj["current_score"] = score;
    smsObj["reason"] = sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD";

    JsonObject waObj = notifObj["whatsapp"].to<JsonObject>();
    waObj["status"] = sendNotif ? "SENT" : "NOT SENT";
    waObj["recipient"] = ALERT_WHATSAPP;
    waObj["threshold"] = 60;
    waObj["current_score"] = score;
    waObj["reason"] = sendNotif ? "NOTIFICATION TRIGGERED" : "SCORE BELOW THRESHOLD";

    // 8. firebase_sync
    JsonObject syncObj = doc["firebase_sync"].to<JsonObject>();
    syncObj["status"] = "CONNECTED";
    syncObj["upload"] = "SUCCESS";
    syncObj["sync_count"] = syncCount;
    syncObj["last_update"] = timeStr;

    // Formatted SMS Alert Template
    String smsMsg = String("\xF0\x9F\x9A\xA8 SENTINELAI-X ALERT\n\nLab 2 ") + threatData.eventName + " Detected!\n\n" +
                    "Time: " + timeStr + "\n" +
                    "Source: " + threatData.eventSource + "\n" +
                    "Motion: " + (hasMotion ? "DETECTED" : "CLEAR") + "\n" +
                    "Threat Score: " + String(score) + "/100\n" +
                    "Risk: " + threatData.overallRisk + "\n" +
                    "Status: " + threatData.eventStatus + "\n\n" +
                    "Please check the laboratory.";
    notifObj["sms_template"] = smsMsg;

    // Formatted Email Alert Template
    String emailSubj = String("\xF0\x9F\x9A\xA8 SentinelAI-X \xE2\x80\x94 ") + threatData.eventName + " Alert | Lab 2";
    String emailMsg = String("SENTINELAI-X SECURITY ALERT\n\nA possible intrusion has been detected in Lab 2.\n\nDate: ") +
                      dateStr + "\n" +
                      "Time: " + timeStr + "\n" +
                      "Mode: " + String(sensorData.timeMode == TimeMode::DAY ? "DAY" : "NIGHT") + "\n" +
                      "Security: " + String(sensorData.securityArmed ? "ARMED" : "DISARMED") + "\n\n" +
                      "EVENT\nSource: " + threatData.eventSource + "\n" +
                      "Event: " + threatData.eventName + "\n" +
                      "Motion: " + (hasMotion ? "DETECTED" : "CLEAR") + "\n\n" +
                      "THREAT ANALYSIS\nThreat Score: " + String(score) + "/100\n" +
                      "Threat Level: " + getThreatLevelString(threatData.threatLevel) + "\n" +
                      "Overall Risk: " + threatData.overallRisk + "\n\n" +
                      "SENSOR STATUS\nTemperature: " + String(sensorData.temperature, 1) + " \xC2\xB0""C\n" +
                      "Humidity: " + String((int)sensorData.humidity) + " %\n" +
                      "Gas: " + String(sensorData.gasADC) + "\n" +
                      "Motion: " + (hasMotion ? "DETECTED" : "CLEAR") + "\n" +
                      "Button: " + (sensorData.buttonPressed ? "PRESSED" : "RELEASED") + "\n\n" +
                      "SYSTEM RESPONSE\nBuzzer: " + (isBuzzerOn ? "ON" : "OFF") + "\n" +
                      "Red LED: " + redLedStr + "\n" +
                      "Alarm: " + alarmModeStr + "\n\n" +
                      "Please check Lab 2 immediately.";

    notifObj["email_subject"] = emailSubj;
    notifObj["email_template"] = emailMsg;

    // Formatted WhatsApp Alert Template
    String waMsg = String("\xF0\x9F\x9A\xA8 *SENTINELAI-X SECURITY ALERT*\n\n\xE2\x9A\xA0\xEF\xB8\x8F *") +
                   threatData.eventName + " DETECTED*\n\n" +
                   "\xF0\x9F\x93\x8D Location: Lab 2\n" +
                   "\xF0\x9F\x95\x90 Time: " + timeStr + "\n" +
                   "\xF0\x9F\x94\x90 Security: " + String(sensorData.securityArmed ? "ARMED" : "DISARMED") + "\n\n" +
                   "\xF0\x9F\x94\x8E *Detection*\n" +
                   "\xE2\x80\xA2 Source: " + threatData.eventSource + "\n" +
                   "\xE2\x80\xA2 Motion: " + (hasMotion ? "DETECTED" : "CLEAR") + "\n" +
                   "\xE2\x80\xA2 Event: " + threatData.eventName + "\n\n" +
                   "\xF0\x9F\x93\x8A *Threat Analysis*\n" +
                   "\xE2\x80\xA2 Score: " + String(score) + "/100\n" +
                   "\xE2\x80\xA2 Threat: " + getThreatLevelString(threatData.threatLevel) + "\n" +
                   "\xE2\x80\xA2 Overall Risk: " + threatData.overallRisk + "\n\n" +
                   "\xF0\x9F\x8C\xA1 Temperature: " + String(sensorData.temperature, 1) + " \xC2\xB0""C\n" +
                   "\xF0\x9F\x92\xA7 Humidity: " + String((int)sensorData.humidity) + "%\n" +
                   "\xF0\x9F\xA9\xAA Gas: " + String(sensorData.gasADC) + "\n\n" +
                   "\xF0\x9F\x94\x94 *Response*\n" +
                   "\xE2\x80\xA2 Buzzer: " + (isBuzzerOn ? "ON" : "OFF") + "\n" +
                   "\xE2\x80\xA2 Red LED: " + redLedStr + "\n" +
                   "\xE2\x80\xA2 Alarm: " + alarmModeStr + "\n\n" +
                   "\xE2\x9A\xA0\xEF\xB8\x8F Please check Lab 2.";

    notifObj["whatsapp_template"] = waMsg;



    String jsonPayload;
    serializeJson(doc, jsonPayload);

    bool success = false;

    // Try Firebase library upload to /SentinelAI-X and /Lab2
    if (Firebase.ready())
    {
        FirebaseJson fbJson;
        fbJson.setJsonData(jsonPayload);
        if (Firebase.setJSON(fbdo, "/SentinelAI-X", fbJson))
        {
            firebaseStatus = FirebaseStatus::FIREBASE_CONNECTED;
            success = true;
        }
        Firebase.setJSON(fbdo, "/Lab2", fbJson);
    }

    // HTTP REST API Fallback
    if (!success && WiFi.status() == WL_CONNECTED)
    {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        String url = String(FIREBASE_URL) + "/SentinelAI-X.json";
        http.begin(client, url);
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.PUT(jsonPayload);
        http.end();

        if (httpResponseCode == 200 || httpResponseCode == 204)
        {
            firebaseStatus = FirebaseStatus::FIREBASE_CONNECTED;
            success = true;

            HTTPClient httpLab;
            httpLab.begin(client, String(FIREBASE_URL) + "/Lab2.json");
            httpLab.addHeader("Content-Type", "application/json");
            httpLab.PUT(jsonPayload);
            httpLab.end();
        }
    }

    if (!success)
    {
        firebaseStatus = FirebaseStatus::FIREBASE_ERROR;
    }

    return success;
}

bool FirebaseManager::uploadSensorData(const SensorData& sensorData)
{
    return true;
}

bool FirebaseManager::uploadThreatData(const ThreatData& threatData)
{
    return true;
}

bool FirebaseManager::uploadAlertData(const AlertData& alertData)
{
    return true;
}

FirebaseStatus FirebaseManager::getStatus() const
{
    return firebaseStatus;
}

bool FirebaseManager::isConnected() const
{
    return firebaseStatus == FirebaseStatus::FIREBASE_CONNECTED;
}
