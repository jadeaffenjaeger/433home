#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>

// set up WiFi access
const char *ssid =      "SSID";
const char *password =  "PASSWORD";

// set up static IP
IPAddress           ip(192,168,0,10);
IPAddress           gateway(192,168,0,1);
IPAddress           subnet(255,255,255,0);

ESP8266WebServer    server(80);

typedef struct {
    unsigned int command;
    String name;
    short state;
} Socket;

const int num_sockets = 3;
Socket sockets[num_sockets];

// show how much space is used in Filesystem (total and percent)
String getFSInfo() {
    char buf[40];
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    long used = fs_info.usedBytes;
    long total = fs_info.totalBytes;
    long percent = (100*used)/total;
    sprintf(buf,"%d kiB / %d kiB [%d%%]", used/1024, total/1024, percent);
    return buf;
}

// show current uptime
String getUpTime() {
    long upTime = millis();
    long mins = upTime / 60000;
    long hours = mins / 60;
    long days = hours / 24;
    mins -= hours * 60;
    hours -= days * 24;
    char buf[40];
    sprintf(buf, "%dd %dh %dm", days, hours, mins);
    return buf;
}

// handle invalid request
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++  ) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send ( 404, "text/plain", message  );
}

void handleInfo() {
    Dir dir = SPIFFS.openDir("/");
    String infoHtml;

    File infoFile = SPIFFS.open("/info.html", "r");

    if (infoFile) {
        infoHtml = infoFile.readString();
        infoFile.close();

        infoHtml.replace("[uptime]", getUpTime());
        infoHtml.replace("[rssi]", String(WiFi.RSSI()));
        infoHtml.replace("[FS]", getFSInfo());
        infoHtml.replace("[IP]", WiFi.localIP().toString());

        server.send(200, "text/html", infoHtml);
    } else {
        handleNotFound();
    }
}
// handle index request
void handleRequest() {
    if(server.hasArg("switch")){
        int socketId = server.arg("switch").toInt();
        if(socketId >= 0 && socketId < num_sockets) {
            toggleSocket(socketId);
            Serial.println(String(socketId));
        }
    }

    Dir dir = SPIFFS.openDir("/");
    String indexHtml;

    File indexFile = SPIFFS.open("/index.html", "r");

    if (indexFile) {
        indexHtml = indexFile.readString();
        indexFile.close();

        indexHtml.replace("[buttons]", socketsToHtml());

        server.send(200, "text/html", indexHtml);
    } else {
        handleNotFound();
    }
}

void setup() {

    // connect to Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.config(ip, gateway, subnet);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    server.begin();
    SPIFFS.begin();

    // set up server
    server.onNotFound(handleNotFound);
    server.on("/", handleRequest);
    server.on("/info", handleInfo);
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/favicon.png", SPIFFS, "/favicon.png");

    initSockets();
}

void loop() {
    server.handleClient();
}
