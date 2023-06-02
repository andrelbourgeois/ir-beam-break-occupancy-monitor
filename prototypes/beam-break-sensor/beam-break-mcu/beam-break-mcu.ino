// constants for LED and signal
// signal and nosignal allow better code readability
#define noSIGNAL HIGH
#define SIGNAL HIGH

// declare pin for recieving data from ir sensor
const byte RECPIN = 2;

// declare bool variables for holding current and previous sensor readings
bool inputStatus;
bool lastStatus;

void setup() {
  // open serial connection
  Serial.begin(9600);
  // delay to ensure connection before anything else
  delay(100);

  // set ir recevier as input
  pinMode(RECPIN, INPUT);
}

void loop() {
  // read ir sensor and save to inputstatus
  inputStatus = digitalRead(RECPIN);
  // print value for debugging
  //Serial.println(inputStatus);
  //Serial.println();
  
  if (inputStatus == noSIGNAL && inputStatus != lastStatus) {
    // if nosignal and change in inputstatus, beam has been broken
      Serial.println("BROKEN");
      Serial.println();   
    }

  // update laststatus
  lastStatus = inputStatus;
  
  delay(10);

}
