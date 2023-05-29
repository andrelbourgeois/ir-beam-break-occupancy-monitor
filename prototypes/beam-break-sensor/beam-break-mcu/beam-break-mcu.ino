// constants for LED and signal
//#define LEDon LOW
//#define LEDoff HIGH
#define noSIGNAL LOW
#define SIGNAL HIGH

// declare pins
// const byte LEDPIN = 3; // no LED pin because of 555 timer
// const byte statusLED = 13;
const byte RECPIN = 4;

// declare variable to store most recent sensor status
byte lastSensorStatus;

byte inputStatus;
byte lastStatus;

void setup() {
  // open serial connection
  Serial.begin(9600);
  delay(100);

  // set IR recevier as input
  pinMode(RECPIN, INPUT);
  digitalWrite(RECPIN, noSIGNAL);

  // tone(LEDPIN, 38000); // no tone because of 555, frequency is in the HW
}

void loop() {
  
  inputStatus = digitalRead(RECPIN);
  Serial.println(RECPIN);
  Serial.println();
  
  if (inputStatus == SIGNAL && inputStatus != lastStatus) {
      Serial.println("BROKEN");
      Serial.println();   
    }

  lastStatus = inputStatus;
  
  delay(1000);

}
