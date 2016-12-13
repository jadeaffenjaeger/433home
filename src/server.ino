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

// store buttons
short buttons[] =   {0,0,0};

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

// Print state of button
String getButonState(short id) {
    String state = "pas";
    if (buttons[id] != 0) {
        state = "act";
    }
    return state;
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

// handle index request
void handleRequest() {
    if(server.hasArg("switch")){
        if(server.arg("switch") == "1") toggleSwitch(1);
        if(server.arg("switch") == "2") toggleSwitch(2);
        if(server.arg("switch") == "3") toggleSwitch(3);
    }

    Dir dir = SPIFFS.openDir("/");
    String indexHtml;

    File indexFile = SPIFFS.open("/index.html", "r");

    if (indexFile) {
        indexHtml = indexFile.readString();
        indexFile.close();

        // add dynamic content (Slow...)
        indexHtml.replace("[buttonState1]", getButonState(1));
        indexHtml.replace("[buttonState2]", getButonState(2));
        indexHtml.replace("[buttonState3]", getButonState(3));

        indexHtml.replace("[uptime]", getUpTime());
        indexHtml.replace("[rssi]", String(WiFi.RSSI()));
        indexHtml.replace("[FS]", getFSInfo());
        indexHtml.replace("[IP]", WiFi.localIP().toString());

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
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/favicon.png", SPIFFS, "/favicon.png");

    initSwitches();
}

void loop() {
    server.handleClient();
}
