// imports
#include <ezTime.h> // timestamp
#include <PubSubClient.h> // subscribe and publish to mqtt
#include <ArduinoJson.h> // send and recieve json data
#include <ESP8266WiFi.h> // connect mcu to wifi
#include "mcu_secrets.h" // sensitive info such as wifi and mqtt passwords

// declare constants for IR signal
// signal and nosignal allow for better code readability
#define noSIGNAL HIGH
#define SIGNAL LOW

// declare pins to pull data from ir receievers
const byte RECPIN1 = 2;
const byte RECPIN2 = 4;

// declare bool variables for holding current and previous sensor readings
//beam 1
bool status1;
bool lastStatus1;
// beam 2
bool status2;
bool lastStatus2;

// declare variables to hold the break times of each beam
int breakTime1 = 0;
int breakTime2 = 0;


// declare variable to hold the occupancy count of the space
// set to -1 to account for the +1 from turning system on
int occupancy = -1;

// declare variable to adjust brightness of lightbulb in expo
int brightness = 0;

// declare variable to turn lightbulb on
const char* turnOn = "on";

// wifi info
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

// mqtt info
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
// send data to database for longterm storage
const char* topicDB = "student/andrelb/beam-break";
// send commands to lightbulb in expo
const char* topicLightSet = "student/homeassistant/light/1/set"; // sets brightness, accepts json
const char* topicLightOn = "student/homeassistant/light/1/on"; // turns light on
const char* topicLightOff = "student/homeassistant/light/1/off"; // turns light off
const char* topicLightToggle = "student/homeassistant/light/1/toggle"; // toggles light from on to off

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char msgLightbulb[50];

// date and time
Timezone GB;

void setup() {
  // open serial connection
  Serial.begin(9600);
  // delay ensures connection before anything else
  delay(100);

  // set ir receviers as inputs
  pinMode(RECPIN1, INPUT);
  pinMode(RECPIN2, INPUT);

  // intialize wifi and sync date
  startWifi();
  syncDate();

  // begin mqtt connection
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  lightOn();
  Serial.print("setup complete");
}

void loop() {
  // read ir sensors and save ir receiever status (HIGH or LOW; noSIGNAL or SIGNAL)
  status1 = digitalRead(RECPIN1);
  status2 = digitalRead(RECPIN2);

  // check beam 1
  if (status1 == noSIGNAL && status1 != lastStatus1) { // if nosignal & change in inputstatus, beam has been broken
    Serial.println("BEAM 1 BROKEN");
    // capture time of break for comparison; .now() gets time as an integer
    breakTime1 = GB.now();
    Serial.println(breakTime1);
    Serial.println();
    // check if second beam has been broken recently
    if (breakTime1 > (breakTime2 + 2)) {
      // if not, occupancy inflow
      occupancy += 1;
    }
    else { // else, occupancy outflow
      occupancy -= 1;
    }
    // set lightbulb brightness based on occupancy
    brightness = occupancy * 10;
    sendMQTT();
  }

  else if (status2 == noSIGNAL && status2 != lastStatus2) { // check beam 2
    Serial.println("BEAM 2 BROKEN");
    // capture time for break comparison
    breakTime2 = GB.now();
    Serial.println(breakTime2);
    Serial.println();
  }


  // update laststatus
  lastStatus1 = status1;
  lastStatus2 = status2;

  delay(100);
}

void startWifi() {
  // connect to wifi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if wifi connected, wait until it is
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}

void sendMQTT() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // to mqtt and to database for long term storage and analysis
  snprintf (msg, 50, "%d", occupancy);
  Serial.print("Publish Occupancy: ");
  Serial.println(msg);
  client.publish(topicDB, msg);

  StaticJsonDocument<256> docSend;
  docSend["brightness"] = brightness;
  
  // using buffer helps to allocate memory quicker
  char buffer[256];
  // serialize json document
  serializeJson(docSend, buffer);
  // publish json document
  client.publish(topicLightSet, buffer);
}

// will send commands to mqtt to turn on light
void lightOn() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // to mqtt and to lightbulb
  snprintf (msgLightbulb, 50, "%s", turnOn);
  Serial.print("Publish Occupancy: ");
  Serial.println(msg);
  client.publish(topicLightToggle, msgLightbulb);

  StaticJsonDocument<256> docSend;
  docSend["brightness"] = brightness;
  
  // using buffer helps to allocate memory quicker
  char buffer[256];
  // serialize json document
  serializeJson(docSend, buffer);
  // publish json document
  client.publish(topicLightSet, buffer);
  
  }

void callback(char* topicDB, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topicDB);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/andrelb/beam-break");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}
