#include "mauriceServos.h"
#include "eyes.h"
#include <Arduino_FreeRTOS.h>

#define EYES_SCN_UP 0
#define EYES_SCN_DOWN 1
#define EYES_SCN_LEFT 0
#define EYES_SCN_RIGHT 1

static TaskHandle_t verticalLeftTaskHandle = NULL;
static TaskHandle_t verticalRightTaskHandle = NULL;
static TaskHandle_t horizontalLeftTaskHandle = NULL;
static TaskHandle_t horizontalRightTaskHandle = NULL;


static void eyesVLTask(void *scenario) {
  switch((int)scenario) {
    case EYES_SCN_UP:
      //servoGoTo(JAW, 0); // TODO
      break;
    case EYES_SCN_DOWN:
      //servoGoTo(JAW, 180); // TODO
      break;
  }      
  // Autodelete?
  verticalLeftTaskHandle = NULL;
  vTaskDelete( NULL );  
}

static void eyesVRTask(void *scenario) {
  switch((int)scenario) {
    case EYES_SCN_UP:
      //servoGoTo(JAW, 0); // TODO
      break;
    case EYES_SCN_DOWN:
      //servoGoTo(JAW, 180); // TODO
      break;
  }      
  // Autodelete?
  verticalRightTaskHandle = NULL;
  vTaskDelete( NULL );  
}

static void eyesHLTask(void *scenario) {
  switch((int)scenario) {
    case EYES_SCN_RIGHT:
      //servoGoTo(JAW, 0); // TODO
      break;
    case EYES_SCN_LEFT:
      //servoGoTo(JAW, 180); // TODO
      break;
  }      
  // Autodelete?
  horizontalLeftTaskHandle = NULL;
  vTaskDelete( NULL );  
}

static void eyesHRTask(void *scenario) {
  switch((int)scenario) {
    case EYES_SCN_RIGHT:
      //servoGoTo(JAW, 0); // TODO
      break;
    case EYES_SCN_LEFT:
      //servoGoTo(JAW, 180); // TODO
      break;
  }      
  // Autodelete?
  horizontalRightTaskHandle = NULL;
  vTaskDelete( NULL );  
}


void startVLTask(int scenario) {
  // kill old one if any
  if (verticalLeftTaskHandle != NULL) {
    vTaskDelete(verticalLeftTaskHandle);
  }
  // restart the same
  xTaskCreate(eyesVLTask, "", 2048, (void *)scenario, 10, &verticalLeftTaskHandle);
}

void startVRTask(int scenario) {
  // kill old one if any
  if (verticalRightTaskHandle != NULL) {
    vTaskDelete(verticalRightTaskHandle);
  }
  // restart the same
  xTaskCreate(eyesVRTask, "", 2048, (void *)scenario, 10, &verticalRightTaskHandle);
}
void startHLTask(int scenario) {
  // kill old one if any
  if (horizontalLeftTaskHandle != NULL) {
    vTaskDelete(horizontalLeftTaskHandle);
  }
  // restart the same
  xTaskCreate(eyesHLTask, "", 2048, (void *)scenario, 10, &horizontalLeftTaskHandle);
}
void startHRTask(int scenario) {
  // kill old one if any
  if (horizontalRightTaskHandle != NULL) {
    vTaskDelete(horizontalRightTaskHandle);
  }
  // restart the same
  xTaskCreate(eyesHRTask, "eyesHRTask", 2048, (void *)scenario, 10, &horizontalRightTaskHandle);
}

void eyesUp() {
  startVLTask(EYES_SCN_UP);
  startVRTask(EYES_SCN_UP);
}
void eyesDown() {
  startVLTask(EYES_SCN_DOWN);
  startVRTask(EYES_SCN_DOWN);
}
void eyesLeft() {
  startHLTask(EYES_SCN_LEFT);
  startHRTask(EYES_SCN_LEFT);
}
void eyesRight() {
  startHLTask(EYES_SCN_RIGHT);
  startHLTask(EYES_SCN_RIGHT);
}
