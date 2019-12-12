#include "headStrip.h"
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "mauriceServos.h"
#define PIXEL_PIN    6  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 3  // Number of NeoPixels

Adafruit_NeoPixel headStrip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// boot : full red
// search wifi : red rotating
// wifi ok : full green
// wifi AP : orange
// error : red blink (no wifi or else)

static bool shouldStop = false;
static QueueHandle_t inQueue;
static SemaphoreHandle_t outMutex;

static void stripTask(void *)
{
  int scenario;
  for (;;) {
    xQueueReceive(inQueue, &scenario, portMAX_DELAY);
    shouldStop = false;
    xSemaphoreTake(outMutex, portMAX_DELAY);
    switch(scenario) {           
      case STRIP_BOOT:
        colorWipe(headStrip.Color(255,   0,   0), 50);    // Red
        break;
      case STRIP_WIFI_SEARCH:
        colorWipe(headStrip.Color(255,   0,   0), 50);    // Red
        break;
      case STRIP_WIFI_OK:
        colorWipe(headStrip.Color(0,   255,   0), 50);    // Green
        break;
      case STRIP_WIFI_AP:
        colorWipe(headStrip.Color(0,   0,   255), 50);    // Blue
        break;
      case STRIP_OFF:
        colorWipe(headStrip.Color(0,   0,   0), 50);    // Black/off
        //theaterChase(headStrip.Color(127,   0,   0), 50); // Red
        break;
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
    //Serial.print(scenario);
    xSemaphoreGive(outMutex); // allow execution
  }
}

static uint8_t     mode     = STRIP_BOOT;

void headStripNextScenario() {
  mode++  ;
  if (mode > STRIP_CHASE_RAINBOW) {
    mode = STRIP_BOOT;
  }
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait end of prev scenario
  xSemaphoreGive(outMutex); // allow execution
  
  if (xQueueSend(inQueue, &mode, 2) != pdTRUE) {
    Serial.println("Enqueue error");
  }; // start scenario
}

void headStripSetup() {
  headStrip.begin(); 
  headStrip.show();  // Initialize all pixels to 'off'
  //headStrip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  inQueue = xQueueCreate(2, sizeof(int));
  outMutex = xSemaphoreCreateMutex();
  colorWipe(headStrip.Color(255,   0,   0), 50); // initial scenario
  xTaskCreate(
    stripTask
    ,  "D"  // A name just for humans
    ,  100  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );  
}

void headStripSetMode(uint8_t _mode) {
  mode = _mode;
  if (mode > STRIP_CHASE_RAINBOW || mode < STRIP_BOOT) {
    mode = STRIP_BOOT;
  }
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY);
  xSemaphoreGive(outMutex); // allow execution
  xQueueSend(inQueue, &mode, 2);
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(uint16_t i=0; i<headStrip.numPixels(); i++) { // For each pixel in strip...
    headStrip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    //vTaskDelay(wait);
  }
  headStrip.show();                          //  Update strip to match
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
      if (shouldStop) {
        shouldStop = false;
        return;
      }
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
    if (shouldStop) {
      shouldStop = false;
      return;
    }
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
      if (shouldStop) {
        shouldStop = false;
        return;
      }
      vTaskDelay(wait);
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
