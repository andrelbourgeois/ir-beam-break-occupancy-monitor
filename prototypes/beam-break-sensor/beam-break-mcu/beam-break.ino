#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 4; // GPIO pin 4 (D2) for 38kHz receiver
const uint16_t kIrLedPin = 14; // GPIO pin 14 (D5) for IR LED

IRrecv irrecv(kRecvPin);
decode_results results;

void setup()
{
  Serial.begin(115200);

  irrecv.enableIRIn(); // Start the receiver

  pinMode(kIrLedPin, OUTPUT); // Set up the IR LED pin as an output
  digitalWrite(kIrLedPin, LOW); // Turn off the IR LED
}

void loop()
{
  if (irrecv.decode(&results)) // Check if a code has been received
  {
    Serial.println(results.value, HEX); // Print the received code in hexadecimal format

    irrecv.resume(); // Receive the next code

    // Turn on the IR LED to transmit a signal
    digitalWrite(kIrLedPin, HIGH);
    delay(10); // Wait for a short amount of time
    digitalWrite(kIrLedPin, LOW); // Turn off the IR LED
  }
}
