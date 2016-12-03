#define DATAPIN 2   
#define LONG    875   
#define SHORT   275   

void initSwitches() {
	pinMode(DATAPIN, OUTPUT);
	digitalWrite(DATAPIN, LOW);
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

void toggleSwitch(short id) {
    unsigned long command = 0b1101010100000100000000000;

    switch(id) {
        case 1:
            command |= (0b1101 << 13);
            break;
        case 2:
            command |= (0b0111 << 13);
            break;
        case 3:
            command |= (0b1010 << 13);
            break;
    }

    if(buttons[id] == 0) {
        command |= (0b1100 << 1);
        buttons[id] = 1;
    } else {
        command |= (0b0011 << 1);
        buttons[id] = 0;
    }
    send_command(command);
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
