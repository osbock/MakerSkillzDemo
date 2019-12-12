# MakerSkillzDemo
An object designed to exercise many of the tools at the Garden

## Arduino Code
The main program for this workshop is neopixel-webserver
### Setup
In Arduino set preferences to point the sketchbook folder to the arduino directory of this repo.
[Change Arduino Preferences](./assets/Arduino-prefs.png)

 * save and reopen arduino
 * open neopixel-webserver from the sketchbook menu
 * edit arduino_secrets.h (second tab)
  ** Change #define SECRET_SSID "" \\ put a unique name for your project in the quotes. This will make your project into a unique hotspot that you will connect to to control it, so choose a name others in the class are not likely to use.
 * connect your project via USB to the PC you are using to program it.
 * Tools/Board menu -- select "Adafruit Feather M0"
 * Tools/Port menu -- select the com port that your board is connected to. 
 * press the upload button (left pointing arrow next to the check mark at the top of the arduino window
 
### Connecting
 * from your phone or PC connect to the hotspot name you chose (SECRET_SSID). If your phone warns you that it doesn't have internet, connect anyway.
 * in a browser, type in http://192.168.1.1 (The http is important, as https is the default and we haven't set up the required certificates for that.

