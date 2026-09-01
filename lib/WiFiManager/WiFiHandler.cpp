/******************************************************************************
 * SentinelAI-X
 * File        : WiFiHandler.cpp
 * Module      : WiFi Handler
 * Description : Handles ESP32 WiFi connection using WiFiManager library
 ******************************************************************************/

#include "WiFiHandler.h"



//==============================================================================
// Initialize WiFi Connection
//==============================================================================

void WiFiHandler::begin()
{

    Serial.println();

    Serial.println("==========================================");

    Serial.println("       Initializing WiFi Manager");

    Serial.println("==========================================");


    // Set ESP32 as Station Mode

    WiFi.mode(WIFI_STA);



    // Create WiFiManager object
    // This is from external WiFiManager library

    WiFiManager wm;



    // Auto connect to saved WiFi
    // If no WiFi found, creates AP

    bool result = wm.autoConnect("SentinelAI-X");



    if(result)
    {

        Serial.println();

        Serial.println("WiFi Connected Successfully");


        Serial.print("SSID : ");

        Serial.println(WiFi.SSID());


        Serial.print("IP   : ");

        Serial.println(WiFi.localIP());


    }

    else
    {

        Serial.println();

        Serial.println("WiFi Connection Failed");


        Serial.println("Restarting ESP32...");


        delay(3000);


        ESP.restart();

    }

}



//==============================================================================
// Check WiFi Status
//==============================================================================

bool WiFiHandler::isConnected()
{

    return (WiFi.status() == WL_CONNECTED);

}



//==============================================================================
// Get IP Address
//==============================================================================

String WiFiHandler::getIP()
{

    if(isConnected())
    {

        return WiFi.localIP().toString();

    }


    return "0.0.0.0";

}



//==============================================================================
// Get SSID
//==============================================================================

String WiFiHandler::getSSID()
{

    if(isConnected())
    {

        return WiFi.SSID();

    }


    return "";

}



//==============================================================================
// Reconnect WiFi
//==============================================================================

void WiFiHandler::reconnect()
{

    if(isConnected())
    {

        return;

    }



    Serial.println();

    Serial.println("WiFi Disconnected");

    Serial.println("Attempting Reconnection...");



    WiFi.reconnect();



    unsigned long startTime = millis();



    while(

        WiFi.status() != WL_CONNECTED &&

        millis() - startTime < 15000

    )
    {

        delay(500);

        Serial.print(".");

    }



    Serial.println();



    if(isConnected())
    {

        Serial.println("WiFi Reconnected");


        Serial.print("IP Address : ");

        Serial.println(WiFi.localIP());

    }

    else
    {

        Serial.println("WiFi Reconnection Failed");

    }

}