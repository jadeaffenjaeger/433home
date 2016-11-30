##433 MHz Power Switches Frontend

This is a simple replacement for the remote of a set of wireless power outlets in my home. The ESP8266 serves a webpage and handles POST requests.

###Connection
Simply wire the ESP up with a Serial-USB-Converter and run "pio run -t uploadfs" and "pio run -t upload". After that, OTA update is available. For regular use, connect the data line of a 433MHz transmitter to GPIO2, power to VCC and GND and pull the RST line high.

###Measuring the control sequence
I connected a logic analyzer to the Data line of a 433Mhz receiver module. I simply emulated the measured sequence. Every command is sent multiple times, as the switches don't always pick it up the first time. 10 times seems to work reliably, even with the transmitter running on only 3.3V and the receiver being in another room. For multiple other protocols, have a look at [RC Switch](https://github.com/sui77/rc-switch).

###HTML
There's only one page, which is the html templata in the /data folder. This then gets parsed and the ESP replaces the placeholders with actual values. This is rather slow... moving the info to seperate file should help with that.


