// constants for LED and signal
#define noSIGNAL LOW
#define SIGNAL HIGH

// declare pins
const byte LEDPIN = 3;
const byte RECPIN = 2;

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
  digitalWrite(RECPIN, SIGNAL);
  
  // set IR LED as output
  pinMode(LEDPIN, OUTPUT);

  // use tone function to send 38kHz signal from IR LED
  // designed with tsop38230 IR reciever which recieves at 38kHz
  // change depending on reciever frequency
  tone(LEDPIN, 38000);
}

void loop() {
  
  inputStatus = digitalRead(RECPIN);
  
  if (inputStatus == noSIGNAL && inputStatus != lastStatus) {
      Serial.println("BROKEN");  
    }

  lastStatus = inputStatus;
  
  delay(10);
}

void checkBeam()
{
  // declare a variable to store reading from IR reciever
  byte inputStatus;
  
  // read IR reciever
  
  inputStatus = digitalRead(RECPIN);
}
