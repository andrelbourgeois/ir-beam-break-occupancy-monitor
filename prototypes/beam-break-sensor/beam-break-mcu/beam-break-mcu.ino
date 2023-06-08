// imports
#include <ezTime.h> // timestamp
#include <PubSubClient.h> // subscribe and publish to mqtt
#include <ArduinoJson.h> // send and recieve json data
#include <ESP8266WiFi.h> // connect mcu to wifi
#include "mcu_secrets.h" // sensitive info such as wifi and mqtt passwords

// constants for LED and signal
// signal and nosignal allow better code readability
#define noSIGNAL HIGH
#define SIGNAL HIGH

// declare pin for recieving data from ir sensor
const byte RECPIN = 2;

// declare bool variables for holding current and previous sensor readings
bool inputStatus;
bool lastStatus;

// wifi and mqtt info
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const char* topic = "student/ucfnbou/beam-break1";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// data and time
Timezone GB;

void setup() {
  // open serial connection
  Serial.begin(9600);
  // delay to ensure connection before anything else
  delay(100);

  // set ir recevier as input
  pinMode(RECPIN, INPUT);

  // intialize wifi and sync date
  startWifi();
  syncDate();

  // begin mqtt connection
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
}

void loop() {
  // read ir sensor and save to inputstatus
  inputStatus = digitalRead(RECPIN);
  
  // print value for debugging
  //Serial.println(inputStatus);
  //Serial.println();
  
  if (inputStatus == noSIGNAL && inputStatus != lastStatus) {
    // if nosignal and change in inputstatus, beam has been broken
      sendMQTT();
      Serial.println("BROKEN");
      Serial.println();   
    }

  // update laststatus
  lastStatus = inputStatus;
  
  delay(10);
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

  StaticJsonDocument<256> docSend;
  docSend["beam_status"] = "BROKEN";
  docSend["break_time"] = GB.dateTime();

  // using buffer helps to allocate memory quicker
  char buffer[256];
  // serialize json document
  serializeJson(docSend, buffer);
  // publish json document
  client.publish("student/ucfnbou/beam-break2", buffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonDocument<256> docRec;  // Allocate the JSON document
  deserializeJson(docRec, payload, length);// Deserialize the JSON document
  String myString = String((char*)payload);
  int myValue = docRec["beam_status"];
  Serial.print(myValue);
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
      client.subscribe("student/ucfnbou/medConnected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}
