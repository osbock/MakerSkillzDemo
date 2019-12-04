//HTML
#include "page.h"
//neopixel config
#include <Adafruit_NeoPixel.h>
// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN1    A0
#define LED_PIN2    6

// How many NeoPixels are attached to the Arduino?
//The ring
#define LED_COUNT1 24 
//The panel
#define LED_COUNT2 32

// Declare our NeoPixel ring objects:
Adafruit_NeoPixel ring(LED_COUNT1, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel panel(LED_COUNT2, LED_PIN2, NEO_GRB + NEO_KHZ800);
/*
  WiFi Web Server Neopixel control

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial monitor. 
  If the IP address of your shield is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off


 */

#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  ring.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  panel.begin();
  ring.show();            // Turn OFF all pixels ASAP
  panel.show();
  ring.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  panel.setBrightness(50);

  WiFi.setPins(8,7,4,2);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  Serial.println("Access Point Web Server");

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // by default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}
//globals
bool panelMode = false;
bool ringMode = false;
long lastRingUpdate = 0L;
long lastPanelUpdate = 0L;
#define PANELRATE 100
#define RINGRATE 100
void loop() {
  rainbow();
    
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];

      // a device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(page);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /P" or "GET /R":
        if (currentLine.endsWith("GET /P")) {
          panelMode = !panelMode;    // GET /P toggles the panel
          Serial.print("Panel toggle: ");Serial.println(panelMode);
        }
        if (currentLine.endsWith("GET /R")) {
          Serial.print("Ring toggle: ");Serial.println(ringMode);
          ringMode = !ringMode;     // GET /R toggles the ring
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

// Rainbow cycle along whole ring. 
void rainbow() {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<ring.numPixels(); i++) { // For each pixel in ring...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (ring.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / ring.numPixels());
      // ring.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through ring.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue)));
      if (millis()-lastPanelUpdate > PANELRATE){ 
        lastPanelUpdate = millis();
        if (panelMode){ 
          for (int p =0; p < 4; p++)
            panel.setPixelColor(random(0,32),panel.Color(127,127,127));
        }
        panel.show();
        panel.clear();
      }
  
    }
    if (!ringMode)
      ring.clear();
    ring.show(); // Update strip with new contents
  }
}
