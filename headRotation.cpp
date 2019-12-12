#include "mauriceServos.h"
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#define SCN_LOOK_AROUND   0
#define SCN_LOOK_LEFT     1
#define SCN_LOOK_RIGHT    2
#define SCN_LOOK_FORWARD  3
#define SCN_SAY_NO        4

static bool shouldStop = false;
static QueueHandle_t inQueue;
static SemaphoreHandle_t outMutex;

static void neckRotationTask(void *) {
  int scenario;
  for (;;) {  
    xQueueReceive(inQueue, &scenario, portMAX_DELAY);
    shouldStop = false;
    xSemaphoreTake(outMutex, portMAX_DELAY);
    switch(scenario) {
      case SCN_LOOK_AROUND:
        servoGoTo(HEAD_ROTATION, 90, 20);
        servoGoTo(HEAD_ROTATION, 180, 20);
        servoGoTo(HEAD_ROTATION, 135, 20);
        break;
      case SCN_SAY_NO:
        servoGoTo(HEAD_ROTATION, 120, 5);
        servoGoTo(HEAD_ROTATION, 150, 5);
        servoGoTo(HEAD_ROTATION, 135, 5);
        break;
      case SCN_LOOK_LEFT:
        servoGoTo(HEAD_ROTATION, 45, 20);
        break;
      case SCN_LOOK_RIGHT:
        servoGoTo(HEAD_ROTATION, 215, 20);
        break;
      case SCN_LOOK_FORWARD:
        servoGoTo(HEAD_ROTATION, 135, 20);
        break;
    }
    xSemaphoreGive(outMutex);
  }
}


static uint8_t     mode;
void lookRight() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = SCN_LOOK_RIGHT;
  xQueueSend(inQueue, &mode, 2);
}

void lookLeft() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = SCN_LOOK_LEFT;
  xQueueSend(inQueue, &mode, 2);
}

void lookForward() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = SCN_LOOK_FORWARD;
  xQueueSend(inQueue, &mode, 2);
}

void sayNo() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = SCN_SAY_NO;
  xQueueSend(inQueue, &mode, 2);
}

void lookAround() {
  shouldStop = true;
  xSemaphoreTake(outMutex, portMAX_DELAY); // wait for previous scenario end if anay
  xSemaphoreGive(outMutex); // allow execution
  mode = SCN_LOOK_AROUND;
  xQueueSend(inQueue, &mode, 2);
}

void headRotationSetup() {
  inQueue = xQueueCreate(2, sizeof(int));
  outMutex = xSemaphoreCreateMutex();
  xTaskCreate(
    neckRotationTask
    ,  "H"  // A name just for humans
    ,  100  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );   
}
