# PROTOTYPES
This folder contains the concise documentation for different prototypes that I developed throughout my research. Further information on each prototype including their design and build can be found in each respective folder. <br>
---------------- add chart comparing the different prototypes -------------------------- <br>
---------------- board, deployed, data storage, etc. ----------------------------------- <br>

----------------put materials in little charts with item and amounts--------------------

## Project Directory
1. [Basic Beam-Break Sensor](#basic-beam-break-sensor)
2. [555 Timer Beam-Break](#555-timer-beam-break)
3. [Veroboard Beam-Break](#veroboard-beam-break)
4. [PCB Beam-Break](#pcb-beam-break)

## Prototype 1: Basic Beam Break Sensor
This was the first prototype and is enabled through the Arduino's built in Tone() function and served to prove the viability of the technology in the quickest possible way. The code is straightforward, sending a 38kHz tone from an IR LED to a TSOP38230 IR Reciever, detecting when the signal is broken, and printing a message through a serial connection to acknowledge the break. In this prototype, the arduino is both sending, and recieving the signal.

### Materials
- Arduino Wifi Rev 2 (can be substituted for an Arduino Uno)
- IR LED x1
- TSOP38230 38kHz IR receiver x1
- 10k resistor x1
- Lead cables
- Small breadboard x1

-------------------- add photos --------------------------

## Prototype 2: 555 Timer Beam-Break
This prototype is based around an NE555 precision timer and served to develop a more robust and scalable circuit for the system. The processing within this intelligent piece of hardware uses the resistance and capacitance of its circuit to determine the frequency which to flash the LED. By matching the LED flash and the reception frequency of the IR receiver, motion through the IR beam can be detected. The reciever in this prototype has remained the same but is now connected to a nodeMCU ESP8266, while the 555 timer circuit is being run from a usb power cable. The switch to the ESP8266 was made due to the affordability and smaller footprint of the device.

### Materials
- NodeMCU ESP8266 x1
- Texas Instruments NE555 precision timer x1
- IR LED x1
- TSOP38230 38kHz IR receiver x1
- 10k resistor x1
- 10k Adjustable resistor x1
- 1k resistoer x1
- 100uf capacitor x1
- 0.01uf capacitor x1
- 0.001uf capacitor x1
- Lead cables
- USB power cable
- Large breadboard x1

<br>
----------- add fritzing photos -----------
<br>
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/3d37cc41-50e9-4670-b14c-3be2fbc4067d" style="height:400px;" />
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/1f5482f9-268d-4188-80a7-13dc4b478ce9" style="height:400px;"/>
<br>
Photos of the 555 beam-break prototype, built on a large breadboard; not pictured, the IR receiver.

------ add circuit image ---------

## Prototype 3: Veroboard Beam-Break
This prototype replicates prototype 2 but in a sturdier form, exchanging the breadboard for a solderable veroboard - in this case, the electrcookie solderable breadboard and a piece of miscallaneous stripboard. This will allow for a stable and reliable deployment in order to capture data while prototype 4 is in development. The timer circuit is now powered by a 9V battery, and the reciever circuit continues to be attached a nodeMCU ESP8266, while being powered through a portable battery pack via USB.

### Materials for One Sensor Circuit
- NodeMCU ESP8266 x1
- Texas Instruments NE555 precision timer x1
- IR LED x1
- TSOP38230 38kHz IR receiver x1
- 10k resistor x1
- 10k adjustable resistor x1
- 1k resistoer x1
- 100uf capacitor x1
- 0.01uf capacitor x1
- 0.001uf capacitor x1
- Lead cables
- Small veroboard x1
- Smaller veroboard x1
- 9V battery x1
- 9v battery connector x1
<br>
-------- update images, add receiver images ---------
<br>
-------- add images of circuit diagrams, pinouts, etc ---------
<br>
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/2568b259-e17c-4ca5-a091-03b8e80cdc08" style="height:400px;" />
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/4bfb9f14-5b99-434f-a9b6-d0deb6a123b8" style="height:400px;" />
<br>
Photos of the first veroboard beam-break prototype, built on an Electrocookie solderable breadboard; doesn't include cabling for power.
<br>

### Deployment, Version 1
As this is the first deployed prototype, this was the first to contain a pair of separate sensors, each comprised of one LED and one IR receiver. A paired approached allows the system to determine the direction of movement between the beams, by comparing their break times. This deployment served as a basis to understand the interactions between the LED and receivers, such as their range and levels of interference between sensor couples.

#### Materials for Deployment, Version 1
- Cardboard
- Tape
- Neodymium magnets (used for weight and balance)

### Connections: MQTT


### Deployment, Version 2
This deployment is similar to the first iteration, but served to

#### Materials for Deployment Version 2
- 3D printed stands
- Neodmymium magnets

### Connections: Azure
The second deployment also provided a more robust and scalable means of data storage, by leveraging the Azure cloud environment. Data enters the pipeline and is routed from the device through MQTT to Azure IoT Hub. From there, the data continues to an Azure Cosmos Database for long-term storage.


---------------- data pipeline diagram -----------------------

<br>
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/5b4b36f9-9f3d-4457-9ecd-c7b2c72df90a" style="height:400px;" />
<img src="https://github.com/andrelbourgeois/CASA0022/assets/33913141/9d147aeb-5025-4483-8d9e-400a23a78b43" style="height:400px;" />
<br>
Photos of the veroboard beam-break prototype, including 9v battery power; deployed on cardboard.

## Prototype 4: PCB Beam-Break
-------- add images of circuit diagrams, pcb layout, etc ---------
<img src="" style="height:400px;" />
<img src="" style="height:400px;" />
<img src="" style="height:400px;" />

