#define DATAPIN 2

#define LONG    875
#define SHORT   275


void initSockets() {
    pinMode(DATAPIN, OUTPUT);
    digitalWrite(DATAPIN, LOW);

    sockets[0].command = 0b11010100;
    sockets[1].command = 0b01110100;
    sockets[2].command = 0b01011100;
}

void switchSocket(short id, bool on) {
    unsigned long raw_command = 0b1101010100000000000000000;

    if(id < 0 || id >= num_sockets)
        return;

    raw_command |= ((sockets[id].command) << 9);

    // Turn on or off
    if(on) {
        raw_command |= (0b1100 << 1);
    } else {
        raw_command |= (0b0011 << 1);
    }

    Serial.println("Sending command: " + String(raw_command));
    send_command(raw_command);
}

void send_command(unsigned long seq) {
    for (int i = 0; i < 10; i++) {
        for(int i = 24; i >= 0; i--) {
            if ((seq >> i) & 0b1) {
                send_one();
            } else {
                send_zero();
            }
        }
        delay(10);
    }
}

void send_one() {
    digitalWrite(DATAPIN, HIGH);
    delayMicroseconds(LONG);
    digitalWrite(DATAPIN, LOW);
    delayMicroseconds(SHORT);
}

void send_zero() {
    digitalWrite(DATAPIN, HIGH);
    delayMicroseconds(SHORT);
    digitalWrite(DATAPIN, LOW);
    delayMicroseconds(LONG);
}
