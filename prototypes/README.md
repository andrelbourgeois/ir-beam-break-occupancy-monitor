# PROTOTYPES
This folder contains the concise documentation for different prototypes that I developed throughout my research. Further information on each prototype including their design and build can be found in each respective folder.

(put materials in little charts with item and amounts)

## Project Directory
1. [Basic Beam-Break Sensor](#basic-beam-break-sensor)
2. [555 Timer Beam-Break](#555-timer-beam-break)
3. [Veroboard Beam-Break](#veroboard-beam-break)

## Basic Beam Break Sensor
This was the first prototype and is enabled through the Arduino's built in Tone() function. The code is straightforward, sending a 38kHz tone from an IR LED to a TSOP38230 IR Reciever, detecting when the signal is broken, and printing a message through a serial connection to acknowledge the break. In this prototype, the arduino is both sending, and recieving the signal.

### Materials
- Arduino Wifi Rev 2 (can be substituted for an Arduino Uno)
- IR LED x1
- TSOP38230 38kHz IR Receiver x1
- 10k Resistor x1
- Lead Cables

## 555 Timer Beam-Break
This was the second prototype and is enabled through with a 555 timer. The processing within this intelligent piece of hardware uses the resistance and capacitance of its circuit to determine the frequency which to light the LED.
-----------add fritzing photos-----------
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/3d37cc41-50e9-4670-b14c-3be2fbc4067d" style="height:400px;" />
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/1f5482f9-268d-4188-80a7-13dc4b478ce9" style="height:400px;"/>
<br>
Photos of the 555 beam-break prototype, built using a large breadboard; not pictured, the IR receiver.

### Materials
- NodeMCU
- Texas Instruments NE555 Precision Timer
- IR LED x1
- TSOP38230 38kHz IR Receiver x1
- 10k Resistor x1
- 10k Adjustable Resistor x1
- 1k Resistoer x1
- 100uf Capacitor x1
- 0.01uf Capacitor x1
- 0.001uf Capacitor x1
- Lead Cables

## Veroboard Beam-Break
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/2568b259-e17c-4ca5-a091-03b8e80cdc08" style="height:400px;" />
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/4bfb9f14-5b99-434f-a9b6-d0deb6a123b8" style="height:400px;" />
<br>
Photos of the first veroboard beam-break prototype, built using a large breadboard; not pictured, the IR receiver.

### Materials
- NodeMCU
- Texas Instruments NE555 Precision Timer
- IR LED x1
- TSOP38230 38kHz IR Receiver x1
- 10k Resistor x1
- 10k Adjustable Resistor x1
- 1k Resistoer x1
- 100uf Capacitor x1
- 0.01uf Capacitor x1
- 0.001uf Capacitor x1
- Lead Cables

