#include <LedControl.h>

const int DIN_PIN = 11;
const int CS_PIN = 13;
const int CLK_PIN = 12;

const uint64_t IMAGES[] PROGMEM = {
  0x70c8c8c8c8c8c870,
  0xf060606060607060,
  0xf8183060c0c0c870,
  0x70c8c0c060c0c870,
  0xc0c0c0c0f8c8c8c8,
  0x70c8c0c0781818f8,
  0x7098989878189870,
  0x3030303060c0c0f8,
  0x70c8c8c870c8c870,
  0x70c8c0f0c8c8c870,
};
const int IMAGES_LEN = sizeof(IMAGES) / 8;


LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 3);

void show(uint8_t hh, uint8_t mm) {
  uint8_t tm[4];
  tm[0] = hh / 10; tm[1] = hh % 10;
  tm[2] = mm / 10; tm[3] = mm % 10;

  uint8_t digit[4][8];
  for (int i = 0; i < 4; i++)
    memcpy_P(&digit[i], &IMAGES[tm[i]], 8);

  for (int i = 0; i < 8; i++) {
    byte row;

    row = digit[0][i] >> 3 | digit[1][i] << 3;
    display.setColumn(0, i, row);

    row = digit[1][i] >> 5 | digit[2][i] << 2;
    display.setColumn(1, i, row);

    row = digit[2][i] >> 6 | digit[3][i];
    display.setColumn(2, i, row);
  }
}

void setup() {
  Serial.begin(9600);

  //we have already set the number of devices when we created the LedControl
  int devices = display.getDeviceCount();
  //we have to init all devices in a loop
  for (int address = 0; address < devices; address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    display.shutdown(address, false);
    /* Set the brightness to a medium values */
    display.setIntensity(address, 2);
    /* and clear the display */
    display.clearDisplay(address);
  }

  show(14, 06);
}


void loop() {

}
