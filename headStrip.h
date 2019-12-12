#include <Adafruit_NeoPixel.h>

void headStripSetup();
void headStripNextScenario();
void theaterChase(uint32_t color, int wait);
void rainbow(int wait);
void theaterChaseRainbow(int wait);
void colorWipe(uint32_t color, int wait);

#define STRIP_BOOT              0
#define STRIP_WIFI_SEARCH       1
#define STRIP_WIFI_OK           2
#define STRIP_WIFI_AP           3
#define STRIP_ERROR1            4
#define STRIP_OFF               5
#define STRIP_MOOVING           6
#define STRIP_RAINBOW           7
#define STRIP_CHASE_RAINBOW     8




