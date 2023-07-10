// imports
#include <ezTime.h> // timestamp
#include <PubSubClient.h> // subscribe and publish to mqtt
#include <ArduinoJson.h> // send and recieve json data
#include <ESP8266WiFi.h> // connect mcu to wifi
#include "esp8266_secrets.h" // sensitive info such as wifi and mqtt passwords

// constants for LED and signal
// signal and nosignal allow better code readability
#define noSIGNAL HIGH
#define SIGNAL HIGH

// declare pins to pull data from ir receievers
const byte RECPIN1 = 26;
const byte RECPIN2 = 33;

// declare bool variables for holding current and previous sensor readings
bool status1;
bool lastStatus1;
// 2nd beam
bool status2;
bool lastStatus2;

// declare variable to hold the break times of each beam
String breakTime1;
String breakTime2;

// declare variable to hold the occupancy count of the space being monitored
int occupancy = 0;

// wifi and mqtt info
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const char* topic = "student/ucfnbou/lab_occupancy";

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

  // set ir receviers as inputs
  pinMode(RECPIN1, INPUT);
  pinMode(RECPIN2, INPUT);

  // intialize wifi and sync date
  startWifi();
  syncDate();

  // begin mqtt connection
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
}

void loop() {
  // read ir sensors and save to status1 and 2
  status1 = digitalRead(RECPIN1);
  status2 = digitalRead(RECPIN2);
  
  // print value for debugging
  //Serial.println(status1);
  //Serial.println();
  //Serial.println(status2);
  //Serial.println();
  
  if (status1 == noSIGNAL && status1 != lastStatus1) {
    // if nosignal and change in inputstatus, beam has been broken
      Serial.println("BEAM 1 BROKEN");
      breakTime1 = GB.dateTime();
      Serial.println(breakTime1);
      Serial.println();
    } else if (status2 == noSIGNAL && status2 != lastStatus2) {
      Serial.println("BEAM 2 BROKEN");
      breakTime2 = GB.dateTime();
      Serial.println(breakTime2);
      Serial.println();  
    };
    
    /*
    if (breakTime1 > breakTime2) {
      count += 1;
    } else if (breakTime2 > breakTime1) {
      count -= 1;
    }
    */
    
    sendMQTT();

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

  StaticJsonDocument<256> docSend;
  docSend["room_count"] = occupancy;
  docSend["count_time"] = GB.dateTime();

  // using buffer helps to allocate memory quicker
  char buffer[256];
  // serialize json document
  serializeJson(docSend, buffer);
  // publish json document
  client.publish("student/ucfnbou/lab_occupancy", buffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  //for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  //}
  //Serial.println();

  StaticJsonDocument<256> docRec;  // Allocate the JSON document
  deserializeJson(docRec, payload, length);// Deserialize the JSON document
  String myString = String((char*)payload);
  int myValue = docRec["beam_status"];
  //Serial.print(myValue);
  //Serial.println();
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
      client.subscribe("student/ucfnbou/lab_occupancy");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}
