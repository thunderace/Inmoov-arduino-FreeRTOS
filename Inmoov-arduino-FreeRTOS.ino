#include <Arduino_FreeRTOS.h>

#include "mauriceServos.h"
#include "headRotation.h"
#include "eyes.h"
#include "headStrip.h"
#include "neck.h"
#include "jaw.h"


void workerTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  Serial.println("Maurice is booting!");
  headStripSetup();
  servoSetup();
  jawSetup();
  xTaskCreate(
    workerTask
    ,  "z"  // A name just for humans
    ,  200  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  Serial.println("Maurice boot complete!");
}

void loop() {
}

void workerTask(void *pvParameters)  // This is a task.
{
  for (;;) {
    Serial.print(".");
    headStripNextScenario();
    Serial.print("-");
    vTaskDelay(200);  // one tick delay (15ms) in between reads for stability
  }
}
