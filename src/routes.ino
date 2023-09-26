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

void handleUptime() {
    long upTime = millis();
    long secs = upTime / 1000;
    long mins = secs / 60;
    long hours = mins / 60;
    long days = hours / 24;
    secs -= mins * 60;
    mins -= hours * 60;
    hours -= days * 24;
    char buf[40];
    snprintf(buf, 40, "%d days, %02dh:%02dm", days, hours, mins);
    server.send(200, "text/plain", String(buf));
}

void handleSSID() {
    server.send(200, "text/plain", String(WiFi.SSID()));
}

void handleRSSI() {
    server.send(200, "text/plain", String(WiFi.RSSI()) + " dBm");
}

void handleIP() {
    server.send(200, "text/plain", String(WiFi.localIP().toString()));
}

void handleFsInfo() {
    char buf[40];
    FSInfo fs_info;
    LittleFS.info(fs_info);
    long used = fs_info.usedBytes;
    long total = fs_info.totalBytes;
    long percent = (100*used)/total;
    snprintf(buf, 40, "%d kiB / %d kiB [%d%%]", used/1024, total/1024, percent);
    server.send(200, "text/plain", String(buf));
}

void handleSocket() {
    if (server.args() > 0 ) { 
        for ( uint8_t i = 0; i < server.args(); i++ ) {
            int socketId = server.argName(i).toInt();
            bool switch_on = server.arg(i) == "on";
            if(socketId > 0 && socketId <= num_sockets) {
                switchSocket(socketId -1, switch_on);
            }
        }
        server.send(200, "text/plain", String("OK."));
    } else {
        server.send(404, "text/plain", String("Error"));
    }
}
