# PROTOTYPES
This folder contains the concise documentation for different prototypes that I developed throughout my research. Further information on each prototype including their design and build can be found in each respective folder.

## Project Directory
- [Basic Beam Break Sensor](#basic-beam-break-sensor)

## Basic Beam Break Sensor
This was my first prototype and is enabled through the Arduino's built in Tone() function. The code is straightforward, sending a 38kHz tone from an IR LED to a TSOP38230 IR Reciever, detecting when the signal is broken, and printing a message through a serial connection to acknowledge the break.

### Materials
- Arduino Wifi Rev 2 (can be substituted for an Arduino Uno)
- IR LED x1
- TSOP38230 38kHz IR Receiver x1
- 10k Resistor x1
- Lead Cables
