#include "headStrip.h"
#include <Arduino_FreeRTOS.h>
#include "mauriceServos.h"
#define PIXEL_PIN    6  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 3  // Number of NeoPixels
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel headStrip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> headStrip(PIXEL_COUNT, PIXEL_PIN);
// boot : full red
// search wifi : red rotating
// wifi ok : full green
// wifi AP : orange
// error : red blink (no wifi or else)

void theaterChase(uint32_t color, int wait);
void rainbow(int wait);
void theaterChaseRainbow(int wait);
void colorWipe(uint32_t color, int wait);
void headStripTask();

#define STRIP_BOOT        0
#define STRIP_WIFI_SEARCH 1
#define STRIP_WIFI_OK     2
#define STRIP_WIFI_AP     3
#define STRIP_ERROR1      4
#define STRIP_OFF         5
#define STRIP_MOOVING     6
#define STRIP_RAINBOW     7
#define STRIP_CHASE_RAINBOW     8

static TaskHandle_t taskHandle = NULL;
uint8_t     mode     = STRIP_OFF;

void killTask() {
  if (taskHandle != NULL) {
    vTaskDelete(taskHandle);
    taskHandle = NULL;
  }
}

void startTask() {
  killTask();
    // restart the same
  xTaskCreate(headStripTask, "headStripTask", 2048, NULL, 10, &taskHandle);
}
void headStripNextScenario() {
  mode++  ;
  if (mode > STRIP_CHASE_RAINBOW) {
    mode = STRIP_BOOT;
  }
  switch(mode) {           
    case STRIP_BOOT:
      killTask();
      colorWipe(headStrip.Color(255,   0,   0), 50);    // Red
      break;
    case STRIP_WIFI_SEARCH:
      killTask();
      colorWipe(headStrip.Color(255,   0,   0), 50);    // Red
      break;
    case STRIP_WIFI_OK:
      killTask();
      colorWipe(headStrip.Color(0,   255,   0), 50);    // Green
      break;
    case STRIP_WIFI_AP:
      killTask();
      colorWipe(headStrip.Color(0,   0,   255), 50);    // Blue
      break;
    case STRIP_OFF:
      killTask();
      colorWipe(headStrip.Color(0,   0,   0), 50);    // Black/off
      //theaterChase(headStrip.Color(127,   0,   0), 50); // Red
      break;
    case STRIP_ERROR1:
    case STRIP_MOOVING:
    case STRIP_RAINBOW:
    case STRIP_CHASE_RAINBOW:
      // create task
      break;       
  }
  
}

void headStripTask(void */*pvParameters*/) { // for complex mouvements
  switch(mode) {           
    case STRIP_ERROR1:
      theaterChase(headStrip.Color(127, 127, 127), 50); // White
      break;
    case STRIP_MOOVING:
      theaterChase(headStrip.Color(0,   0,   255), 50); // Blue
      break;
    case STRIP_RAINBOW:
      rainbow(10);
      break;
    case STRIP_CHASE_RAINBOW:
      theaterChaseRainbow(50);
      break;       
  }
}

void headStripSetup() {
  headStrip.begin(); 
  headStrip.show();  // Initialize all pixels to 'off'
  headStrip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void headStripSetMode(uint8_t _mode) {
  mode = _mode;
  mode -= 1;
  headStripNextScenario();
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(uint16_t i=0; i<headStrip.numPixels(); i++) { // For each pixel in strip...
    headStrip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    headStrip.show();                          //  Update strip to match
    vTaskDelay(wait);
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      headStrip.clear();
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint16_t c=b; c<headStrip.numPixels(); c += 3) {
        headStrip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      headStrip.show(); // Update strip with new contents
      vTaskDelay(wait);
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(uint16_t i=0; i<headStrip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.PixelCount() steps):
      int pixelHue = firstPixelHue + (i * 65536L / headStrip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      headStrip.setPixelColor(i, headStrip.gamma32(headStrip.ColorHSV(pixelHue)));
    }
    headStrip.show(); // Update strip with new contents
    vTaskDelay(wait);
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      headStrip.clear();
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(uint16_t c=b; c<headStrip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.PixelCount() steps):
        int      hue   = firstPixelHue + c * 65536L / headStrip.numPixels();
        uint32_t color = headStrip.gamma32(headStrip.ColorHSV(hue)); // hue -> RGB
        headStrip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      headStrip.show();                // Update strip with new contents
      vTaskDelay(wait);
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
