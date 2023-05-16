// constants for LED and signal
//#define LEDon LOW
//#define LEDoff HIGH
#define noSIGNAL LOW
#define SIGNAL HIGH

// declare pins
const byte LEDPIN = 3;
const byte statusLED = 13;
const byte RECPIN = 2;

// declare variable to store most recent sensor status
byte lastSensorStatus;

// timing things
//unsigned long checkBeamMillis;
//unsigned long detectedMillis;

byte inputStatus;
byte lastStatus;

void setup() {
  // open serial connection
  Serial.begin(9600);
  delay(100);

  // pin 13 to output
  //pinMode(statusLED, OUTPUT);
  //digitalWrite(statusLED, LEDoff);

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

  
  // check the beam
  // checkBeam();
  // make sure a little time has passed
  
  /*
  // check the inputs every 20ms
  if (millis() - checkBeamMillis >= 20)
  {
    //restart this TIMER
    checkBeamMillis = millis();

    checkBeam();
  }
  */
}

void checkBeam()
{
  // declare a variable to store reading from IR reciever
  byte inputStatus;
  
  // read IR reciever
  
  inputStatus = digitalRead(RECPIN);
  
  /*
  // check for a change in the sensor status
  if (lastSensorStatus != inputStatus)
  {
    // update lastSensorStatus
    lastSensorStatus = inputStatus;

    // check if the signal disappeared
    if (inputStatus == noSIGNAL)
    {
      digitalWrite(statusLED, LEDon);

      // the time the object went into the IR beam
      detectedMillis = millis();
    }

    // the signal has returned
    else
    {
      digitalWrite(statusLED, LEDoff);

      Serial.print("Object was in beam for ");
      Serial.print(millis() - detectedMillis);
      Serial.println(" milliseconds.");
    }
  } */
}
