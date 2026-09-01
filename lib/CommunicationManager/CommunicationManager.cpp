/******************************************************************************
 * SentinelAI-X
 * File        : CommunicationManager.cpp
 * Module      : Communication Manager
 * Description : Handles Wi-Fi initialization, monitoring, and reconnection.
 ******************************************************************************/

#include "CommunicationManager.h"
#include <WiFiManager.h>

//==============================================================================
// Constructor
//==============================================================================

CommunicationManager::CommunicationManager()
{
}

//==============================================================================
// Initialize Communication Manager
//==============================================================================

void CommunicationManager::begin()
{
    Serial.println();
    Serial.println("========================================");
    Serial.println(" SentinelAI-X Communication Manager");
    Serial.println("========================================");

    connectWiFi();
}

//==============================================================================
// Update Communication Manager
//==============================================================================

void CommunicationManager::update()
{
    if (!isConnected())
    {
        reconnectWiFi();
    }
}

//==============================================================================
// Connect to Wi-Fi
//==============================================================================

bool CommunicationManager::connectWiFi()
{
    WiFi.mode(WIFI_STA);

    WiFiManager wifiManager;

    // Uncomment this line only if you want to erase saved credentials
    // wifiManager.resetSettings();

    bool connected = wifiManager.autoConnect("SentinelAI_Setup");

    if (!connected)
    {
        Serial.println();
        Serial.println("WiFi Connection Failed!");
        Serial.println("Restarting ESP32...");

        delay(3000);
        ESP.restart();

        return false;
    }

    Serial.println();
    Serial.println("WiFi Connected Successfully!");

    printNetworkInfo();

    return true;
}

//==============================================================================
// Reconnect Wi-Fi
//==============================================================================

void CommunicationManager::reconnectWiFi()
{
    Serial.println();
    Serial.println("WiFi Disconnected!");
    Serial.println("Attempting Reconnection...");

    WiFi.reconnect();

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < 10000)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi Reconnected Successfully!");
        printNetworkInfo();
    }
    else
    {
        Serial.println("Reconnect Failed!");
    }
}

//==============================================================================
// Check Wi-Fi Status
//==============================================================================

bool CommunicationManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

//==============================================================================
// Get Connected SSID
//==============================================================================

String CommunicationManager::getSSID()
{
    return WiFi.SSID();
}

//==============================================================================
// Get IP Address
//==============================================================================

String CommunicationManager::getIPAddress()
{
    return WiFi.localIP().toString();
}

//==============================================================================
// Get Wi-Fi Signal Strength
//==============================================================================

int CommunicationManager::getRSSI()
{
    return WiFi.RSSI();
}

//==============================================================================
// Print Network Information
//==============================================================================

void CommunicationManager::printNetworkInfo()
{
    Serial.println();
    Serial.println("========================================");
    Serial.println(" Network Information");
    Serial.println("========================================");

    Serial.print("SSID           : ");
    Serial.println(getSSID());

    Serial.print("IP Address     : ");
    Serial.println(getIPAddress());

    Serial.print("Gateway        : ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("Subnet Mask    : ");
    Serial.println(WiFi.subnetMask());

    Serial.print("MAC Address    : ");
    Serial.println(WiFi.macAddress());

    Serial.print("Signal RSSI    : ");
    Serial.print(getRSSI());
    Serial.println(" dBm");

    Serial.println("========================================");
}