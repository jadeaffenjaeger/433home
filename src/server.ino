#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "credentials.h"

ESP8266WebServer server(80);

typedef struct
{
    unsigned int command;
} Socket;

const int num_sockets = 3;
Socket sockets[num_sockets];

void setup()
{
    // connect to Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.hostname("433home");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    server.begin();
    LittleFS.begin();
    Serial.begin(115200);

    // set up server
    server.on("/socket", handleSocket);
    server.on("/rssi", handleRSSI);
    server.on("/ssid", handleSSID);
    server.on("/uptime", handleUptime);
    server.on("/ip", handleIP);
    server.on("/fsinfo", handleFsInfo);

    server.serveStatic("/", LittleFS, "/");

    server.onNotFound(handleNotFound);

    initSockets();
}

void loop()
{
    server.handleClient();
}
