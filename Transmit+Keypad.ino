#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include "Wire.h"
#include "I2CKeyPad.h"
#include <LCDWIKI_GUI.h> 
#include <SSD1283A.h> //Hardware-specific library

RF24 radio(6, 7); // CE, CSN
const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad(KEYPAD_ADDRESS);

const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  keyPad.begin(); // Starts I2C keypad interface.
  radio.begin(); // Starts the radio.
  radio.openWritingPipe(address); // Sets the radio channel.
  radio.setPALevel(RF24_PA_MIN); // Sets the radio power.
  radio.stopListening(); // Tell the radio not to listen for messages.
}

void loop() {
  char keys[] = "D#0*C987B654A321NF";  // N = Nokey, F = Fail
  uint8_t idx = keyPad.getKey();
  if ( (keys[idx] != 'N') && (keys[idx] != 'F')) {
    radio.write(&keys[idx], sizeof(keys[idx]));
  }
  delay(1000);
}