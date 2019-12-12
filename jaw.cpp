#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "mauriceServos.h"
#include "jaw.h"

#define JAW_SCN_OPEN 0
#define JAW_SCN_CLOSE 1
#define JAW_SCN_TALK 2

static bool shouldStop = false;
static QueueHandle_t inQueue;
static SemaphoreHandle_t outMutex;

void jawTask(void *) {
  int scenario;
  for (;;) {  
    xQueueReceive(inQueue, &scenario, portMAX_DELAY);
    shouldStop = false;
    xSemaphoreTake(outMutex, portMAX_DELAY);
    switch(scenario) {           
      case JAW_SCN_OPEN:
        //servoGoTo(JAW, 0); // TODO
        break;
      case JAW_SCN_CLOSE:
        //servoGoTo(JAW, 180); // TODO
        break;
      case JAW_SCN_TALK:
      // servoGoTo(JAW, 150); //TODO
      //servoGoTo(JAW, 0); // TODO
       break;
    }
    //Serial.print(scenario);
    xSemaphoreGive(outMutex);
  }
}

static uint8_t     mode;

void openMouth() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = JAW_SCN_OPEN;
  xQueueSend(inQueue, &mode, 2);
}

void closeMouth() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY);// wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = JAW_SCN_CLOSE;
  xQueueSend(inQueue, &mode, 2);
}

void talk() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = JAW_SCN_TALK;
  xQueueSend(inQueue, &mode, 2);
}

void jawSetup() {
  inQueue = xQueueCreate(2, sizeof(int));
  outMutex = xSemaphoreCreateMutex();
  xTaskCreate(
    jawTask
    ,  "J"  // A name just for humans
    ,  100  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );   
}