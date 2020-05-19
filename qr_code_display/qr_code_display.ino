#include <qrcode.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// for hardware SPI
const int OLED_DC = 6;
const int OLED_CS = 7;
const int OLED_RESET = 8;

Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

static const unsigned char PROGMEM qr_bmp[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xf0, 0x00, 0x3c, 0x3f, 
0x00, 0x03, 0xc0, 0xf0, 0x00, 0x3c, 0x3f, 0x00, 0x03, 0xc0, 0xf3, 0xff, 0x33, 0x03, 0x3f, 0xf3, 
0xc0, 0xf3, 0xff, 0x33, 0x03, 0x3f, 0xf3, 0xc0, 0xf3, 0x03, 0x3c, 0xc3, 0x30, 0x33, 0xc0, 0xf3, 
0x03, 0x3c, 0xc3, 0x30, 0x33, 0xc0, 0xf3, 0x03, 0x30, 0xc3, 0x30, 0x33, 0xc0, 0xf3, 0x03, 0x30, 
0xc3, 0x30, 0x33, 0xc0, 0xf3, 0x03, 0x3f, 0x0f, 0x30, 0x33, 0xc0, 0xf3, 0x03, 0x3f, 0x0f, 0x30, 
0x33, 0xc0, 0xf3, 0xff, 0x30, 0x03, 0x3f, 0xf3, 0xc0, 0xf3, 0xff, 0x30, 0x03, 0x3f, 0xf3, 0xc0, 
0xf0, 0x00, 0x33, 0x33, 0x00, 0x03, 0xc0, 0xf0, 0x00, 0x33, 0x33, 0x00, 0x03, 0xc0, 0xff, 0xff, 
0xff, 0x3f, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xc0, 0xf0, 0x03, 0x00, 0xcc, 
0xcc, 0xcf, 0xc0, 0xf0, 0x03, 0x00, 0xcc, 0xcc, 0xcf, 0xc0, 0xf3, 0xcc, 0xc3, 0xfc, 0x00, 0x03, 
0xc0, 0xf3, 0xcc, 0xc3, 0xfc, 0x00, 0x03, 0xc0, 0xf0, 0xf0, 0x0c, 0xf0, 0x0f, 0x0f, 0xc0, 0xf0, 
0xf0, 0x0c, 0xf0, 0x0f, 0x0f, 0xc0, 0xff, 0x0f, 0xc3, 0xf0, 0xf0, 0x3f, 0xc0, 0xff, 0x0f, 0xc3, 
0xf0, 0xf0, 0x3f, 0xc0, 0xff, 0x3f, 0x03, 0x3f, 0x30, 0xf3, 0xc0, 0xff, 0x3f, 0x03, 0x3f, 0x30, 
0xf3, 0xc0, 0xff, 0xff, 0xf0, 0xcc, 0xc0, 0x33, 0xc0, 0xff, 0xff, 0xf0, 0xcc, 0xc0, 0x33, 0xc0, 
0xf0, 0x00, 0x33, 0xc3, 0x0f, 0x0f, 0xc0, 0xf0, 0x00, 0x33, 0xc3, 0x0f, 0x0f, 0xc0, 0xf3, 0xff, 
0x3c, 0x00, 0xc0, 0x3f, 0xc0, 0xf3, 0xff, 0x3c, 0x00, 0xc0, 0x3f, 0xc0, 0xf3, 0x03, 0x30, 0xff, 
0x00, 0xff, 0xc0, 0xf3, 0x03, 0x30, 0xff, 0x00, 0xff, 0xc0, 0xf3, 0x03, 0x30, 0x3f, 0x33, 0x0f, 
0xc0, 0xf3, 0x03, 0x30, 0x3f, 0x33, 0x0f, 0xc0, 0xf3, 0x03, 0x33, 0xc0, 0xcf, 0x3f, 0xc0, 0xf3, 
0x03, 0x33, 0xc0, 0xcf, 0x3f, 0xc0, 0xf3, 0xff, 0x30, 0x3c, 0xf0, 0x3f, 0xc0, 0xf3, 0xff, 0x30, 
0x3c, 0xf0, 0x3f, 0xc0, 0xf0, 0x00, 0x30, 0xf0, 0x0c, 0xcf, 0xc0, 0xf0, 0x00, 0x30, 0xf0, 0x0c, 
0xcf, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0
};

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.drawBitmap(0,0, qr_bmp, 50, 50, SSD1306_WHITE);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

}
