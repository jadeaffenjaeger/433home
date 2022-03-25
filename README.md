# 433 MHz Power Switches Frontend

This is a simple network front end for a set of wireless remote power switches in my home. It sets up an HTTP server, drives a 433 MHz transmitter and displays the current status of the power sockets. The design is responsive to work on phones as well as desktop computers.

### Sockets
I got mine from a large retailer in germany, and they are branded "REV". There's only a few protocols for those radio power outlets, but instead of opening it, looking for the radio chip ID and adapting an existing library, I found it quicker and easier to just reverse engineer the timing of the signals with a receiver (they come together with the transmitter for just 1-2 Euros) and a logic analyzer. If you want to take the other route, however, there's an excellent library that covers most wireless control protocols at [RC Switch](https://github.com/sui77/rc-switch).

### Control sequences
Power up the receiver and connect a logic analyzer to the data line. Then press buttons on the remote and capture the sequence. Take note of the timinig. I only captured three on and three off signals (since I only have three power sockets). The encoding seems to be a return zero code, where a "1" is encoded by the data line being high for 875us and then low for 275us, and the "0" is encoded by a high for 275us and then low for 875us. A command has 25 bit, where the 3rd nibble
seems to encode the channel id, and the 6th nibble (counting from left to right) is either on or off.

This is what one command looks like on the analyzer output:
![Sample trace output](/images/trace_1_on.png)

I repeated the process for the other buttons on the remote as well and those are the sequences I measured:

    1 ON    1101 0101 1101 0100 0000 1100 0
    1 OFF   1101 0101 1101 0100 0000 0011 0

    2 ON    1101 0101 0111 0100 0000 1100 0
    2 OFF   1101 0101 0111 0100 0000 0011 0

    3 ON    1101 0101 0101 1100 0000 1100 0
    3 OFF   1101 0101 0101 1100 0000 0011 0

The remote sends each control sequence 5 times in a row, but I went with 10 times, because my transmitter only runs on 3.3V and the signal needs to pass through walls. From what I can tell, this seems to work quite reliably. The transmitter could take up to 12V, if more range is needed.

### HTML
Update 2022: I gave the whole project a bigger overhaul. The frontend now uses [Bootstrap 5](https://getbootstrap.com) for CSS and [HTMX](https://htmx.org/) to interact with the ESP8266 Webserver. Buttons are now stateless because I found that changing the state of the button depending on whether the socket was switched on or off was more confusing than helpful. HTML is not rendered on the ESP8266 anymore because dynamically replacing content inside files turned out rather slow. Instead, runtime
information is fetched from dedicated endpoints and rendered on the client side via HTMX.

This is what the whole thing looks like on small device:

![Page on phone](/images/page_phone.png)

And that's what it looks like in a browser. 

![Page on desktop](/images/page_desktop.png)

### Building & Flashing
Create a file `src/credentials.h` with the following content:
```
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

#endif
```
Building and flashing is handled by [PlatformIO](https://platformio.org/). Change the line `board = esp01` in the `platformio.ini` if you are using another board. To flash the code, run:
```
pio run 
```
Uploading the file system is handled separately by the command
```
pio run -t uploadfs
```
If you want to open the serial monitor, run:
```
pio run -t monitor
```
