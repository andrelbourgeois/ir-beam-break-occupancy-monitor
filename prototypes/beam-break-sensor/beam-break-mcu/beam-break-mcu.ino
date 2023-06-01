// constants for LED and signal
//#define LEDon LOW
//#define LEDoff HIGH
#define noSIGNAL HIGH
#define SIGNAL HIGH

// declare pins
const byte RECPIN = 2;

bool inputStatus;
bool lastStatus;

void setup() {
  // open serial connection
  Serial.begin(9600);
  delay(100);

  // set IR recevier as input
  pinMode(RECPIN, INPUT);
  //digitalWrite(RECPIN, SIGNAL);

}

void loop() {
  
  inputStatus = digitalRead(RECPIN);
  Serial.println(inputStatus);
  //Serial.println(RECPIN);
  //Serial.println();
  
  //if (inputStatus == noSIGNAL && inputStatus != lastStatus) {
  if (inputStatus == noSIGNAL) {
      Serial.println("BROKEN");
      Serial.println();   
    }

  lastStatus = inputStatus;
  
  delay(1000);

}
