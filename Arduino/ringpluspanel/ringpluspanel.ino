
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



// setup() function -- runs once at startup --------------------------------

void setup() {
  
  // END of Trinket-specific code.

  ring.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  panel.begin();
  ring.show();            // Turn OFF all pixels ASAP
  panel.show();
  ring.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  panel.setBrightness(50);
}


// loop() function -- runs repeatedly as long as board is on ---------------
long lastRingUpdate = 0L;
long lastPanelUpdate = 0L;
#define PANELRATE 100
#define RINGRATE 100
void loop() {

    
  if ((millis() - lastRingUpdate) > RINGRATE){
    lastRingUpdate = millis();
    rainbow();
  }

  
}



// Rainbow cycle along whole ring. Pass delay time (in ms) between frames.
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
        for (int p =0; p < 4; p++)
          panel.setPixelColor(random(0,32),panel.Color(127,127,127));
        panel.show();
        panel.clear();
      }
  
    }
    ring.show(); // Update strip with new contents
  }
}
