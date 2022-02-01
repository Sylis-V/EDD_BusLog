#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>
#include "Wire.h"
#include <I2CKeyPad.h>
#include <GFXcanvas16T.h>
#include <SSD1283A.h>

#define MODEL SSD1283A
#define CS 3
#define A0 1
#define RST 2

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

SSD1283A lcd(CS,A0,RST,-1);//cd,cd,rst,led
RF24 radio(6, 7); // CE, CSN
I2CKeyPad keyPad(0x20);

const byte address[6] = "00001";
char keys[] = "D#0*C987B654A321NF";  // N = Nokey, F = Fail
uint8_t w = lcd.width();
uint8_t h = lcd.height();

void setup() {
  Serial.begin(115200);
  Wire.begin(400000);
  lcd.init();
  lcd.fillScreen(BLACK);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }
  keyPad.loadKeyMap(keys);
}

bool keyPress() {
  while (keyPad.isPressed()) {
    delay(50);
  }
}

char radioIn() {
  recheck:
  if (radio.available()) {
    char text = NULL;
    radio.read(&text, sizeof(text));
    if (text == NULL) {
      delay(500);
      goto recheck;
    }
    return text;
  }
}

int keyIn() {
  rekey:
  uint8_t idx = keyPad.getKey();
  if ( (keys[idx] != 'N') && (keys[idx] != 'F')) {
    return keys[idx];
  }
  goto rekey;
}

int busStore(int* bus) {

}

void csPrint(String str, int txt, bool curLn = true, int mX = 0, int mY = 0) {
  auto pos = [&] (int in, int s) {in = in * 5.75 * s; return in;};
  lcd.setTextSize(txt); 
  int len = str.length();
  int px = px = pos(str.length(),txt);
  if (mX != 0) {
    px = mX;
  }
  if (!curLn) {
    lcd.setCursor((w-px)/len, lcd.getCursorY());
  }
  else {
    lcd.setCursor((w-px)/len, mY);
  }
  lcd.print(str);
}

void loop() {
  lcd.fillScreen(RED);
  delay(1000);
  renum:
  lcd.fillScreen(BLACK);
  bool init = false;
  int bus[3] = {0, 0, 0};
  double px{};
  while (!init) { // lcd positions are 15, 54, 93
    char busNum[3] = {NULL,NULL,NULL};
    csPrint("Bus Number\n", 2);
    busNum[0] = keyIn();
    keyPress();
    csPrint(String(busNum[0]),4,false,15);
    busNum[1] = keyIn();
    keyPress();
    csPrint(String(busNum[1]),4,false,54);
    busNum[2] = keyIn();
    keyPress();
    csPrint(String(busNum[2]),4,false,93);
    switch(keyIn()) {
      case 'A':
      init = true;
      keyPress();
      break;
      case 'B':
      keyPress();
      goto renum;
      break;
      default:
      Serial.println("Error: No key response.");
      break;
    }
    keyPress();
  }
  lcd.fillScreen(GREEN);
  delay(1000);
  lcd.fillScreen(BLACK);
  csPrint("Transmitting:\n",1);
  delay(3000);
}
