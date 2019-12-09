#include <Arduino_FreeRTOS.h>

#include "mauriceServos.h"
#include "headRotation.h"
#include "eyes.h"
#include "headStrip.h"
#include "neck.h"
#include "jaw.h"

void setup() {
  Serial.begin(115200);
  printf("Maurice is booting!\n");
  servoSetup();
}

void loop() {

}
