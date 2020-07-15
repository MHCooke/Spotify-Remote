## Spotify Remote
This is a hardware project to allow you to control your Spotify playback using an external remote. Currently this project is using an ESP32 and SSD1306 128x64 display, in the future I intend to change this to a larger colour display that can also show more information (like the album artwork).

## Requirements
This project relies on the following external libraries:
  - Adafruit_GFX
  - Adafruit_SSD1306
  - [QRCode](ricmoo/QRCode)

## To-do:
Use external URL shortening service. Cannot display 216 character URL QR Code on a 64 pixel tall panel. At least not with a pixel pitch <0.5mm.
