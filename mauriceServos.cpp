#include <stdio.h>
#include <Arduino_FreeRTOS.h>

#include <Adafruit_PWMServoDriver.h>
#include "mauriceServos.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define TYPE_SG90           0
#define TYPE_PDI_6221MG_360 1
#define TYPE_ALEXP_30KG     2
#define TYPE_996R           3
#define TYPE_995            4
#define TYPE_RDS3235        5
#define TYPE_PDI_6221MG_270 6
#define TYPE_NONE           7
// servo 0 : (right eye V : // M : 455 - full up : 330 - full down : 600
// servo 5 : (left eye V : // M : 455 - full up : 330 - full down : 600
int servoDefaultPosition[16] = {
#ifdef ASSEMBLY_POSITIONS
  180,  // right eye full up
  90,   // right eye H center
  0,  // top lid full open
  0,    // bottom lid full open : TBD
  90,   // left eye H center
  0,    // left eye full up
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0
#else
  90,   // right eye V center : TBD
  90,   // right eye H center
  90,   // top lid full closed : TBD
  90,   // bottom lid full closed : TBD
  90,   // left eye H center
  90,   // left eye V center : TBD
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0
#endif  
};
/*
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    long divisor = (in_max - in_min);
    if(divisor == 0){
        return -1; //AVR returns -1, SAM returns 0
    }
    return (x - in_min) * (out_max - out_min) / divisor + out_min;
} 
*/ 
int servoType[16] = {
  TYPE_SG90, // M : 455 - full up : 330 - full down : 600
  TYPE_SG90,  // M : 470 - full up : 600 - full down : 330
  TYPE_SG90, 
  TYPE_SG90, 
  TYPE_NONE,
  TYPE_NONE,
  TYPE_NONE,
  TYPE_NONE,
  TYPE_NONE,
  TYPE_ALEXP_30KG, // shoulder left
  TYPE_ALEXP_30KG, // shoulder right
  TYPE_PDI_6221MG_270, // neck front
  TYPE_PDI_6221MG_270,  // neck left
  TYPE_PDI_6221MG_270,  // neck right
  TYPE_996R, // jaw
  TYPE_RDS3235 //head rotation 270
};
                    
//#define ANGLE
#define MIN_POS 1
#define MIDDLE_POS 2
#define MAX_POS 3
int initialServoPosition[16] = { 
  MIDDLE_POS, 
  MIDDLE_POS, 
  MIDDLE_POS, 
  MIDDLE_POS, 
  0,
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  MIN_POS, 
  MIDDLE_POS};

static int servoPosition[16] = { 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int nextPosition[16] = { 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define SERVO_MIN_IDX 0
#define SERVO_MAX_IDX 1
#define SERVO_MIDDLE_IDX 2
#define SERVO_MIN_ANGLE_IDX 3
#define SERVO_MAX_ANGLE_IDX 4

int servoData[8][5] = {
  {300, 600, 450, 0, 180},  // SG90
  {300, 600, 395, -1, -1},  // PDI 360
  {300, 600, 450, 0, 180},  // 30KG
  {100, 570, 335, 0, 180},  // 996R
  {300, 600, 450, 0, 180},  // 995
  {125, 660, 392, 0, 270},  // RDS3235
  {300, 600, 450, 0, 270},  // PDI 270
  {0, 0, 0, 0, 0}           // NONE
};

void dualServoGoTo(int servo1, int servo2, int angle, int speed) {
  int cur1Pos = servoPosition[servo1]; // in  value
  int cur2Pos = servoPosition[servo2]; // in  value
  int next1Pos = map(angle, servoData[servoType[servo1]][SERVO_MIN_ANGLE_IDX], servoData[servoType[servo1]][SERVO_MAX_ANGLE_IDX], servoData[servoType[servo1]][SERVO_MIN_IDX], servoData[servoType[servo1]][SERVO_MAX_IDX]);
  int next2Pos = map(angle, servoData[servoType[servo2]][SERVO_MIN_ANGLE_IDX], servoData[servoType[servo2]][SERVO_MAX_ANGLE_IDX], servoData[servoType[servo2]][SERVO_MIN_IDX], servoData[servoType[servo2]][SERVO_MAX_IDX]);
  printf("Go from : %d to %d\n", cur1Pos, next1Pos);

  if (cur1Pos > next1Pos) {
    for (int i = cur1Pos; i >= next1Pos; i--) {
      setValue(servo1, i);
      setValue(servo2, cur2Pos + i - cur1Pos);
      vTaskDelay(speed / portTICK_PERIOD_MS);
    }
  } 
  if (cur1Pos < next1Pos) {
    for (int i = cur1Pos; i <= next1Pos; i++) {
      setValue(servo1, i);
      setValue(servo2, cur2Pos - i - cur1Pos);
      vTaskDelay(speed / portTICK_PERIOD_MS);
    }
  }
}

void servoGoTo(int servo, int angle, int speed) {
  int curPos = servoPosition[servo]; // in  value
  int nextPos = map(angle, servoData[servoType[servo]][SERVO_MIN_ANGLE_IDX], servoData[servoType[servo]][SERVO_MAX_ANGLE_IDX], servoData[servoType[servo]][SERVO_MIN_IDX], servoData[servoType[servo]][SERVO_MAX_IDX]);

  printf("Go from : %d to %d\n", curPos, nextPos);

  if (curPos > nextPos) {
    for (int i = curPos; i >= nextPos; i--) {
      setValue(servo, i);
      vTaskDelay(speed / portTICK_PERIOD_MS);
    }
  } 
  if (curPos < nextPos) {
    for (int i = curPos; i <= nextPos; i++) {
      setValue(servo, i);
      vTaskDelay(speed / portTICK_PERIOD_MS);
    }
  }
}

void servoSetup() {
  /*
  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  //pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(60);  // Analog servos run at ~50 Hz updates
 
  for (int i = 0; i < 16; i++) {
    switch (initialServoPosition[i]) {
      case MIDDLE_POS:
        setMiddle(i);
        break;
      case MAX_POS:
        setMax(i);
        break;
      case MIN_POS:
        setMin(i);
        break;
      default:
        //setValue(i, initialServoPosition[i]); 
        break;
    }
  }
  */
}

void setMiddle(int servo) {
  setValue(servo, servoData[servoType[servo]][SERVO_MIDDLE_IDX]);
}

int getMiddle(int servo) {
    return servoData[servoType[servo]][SERVO_MIDDLE_IDX];
}

void setMin(int servo) {
  setValue(servo, servoData[servoType[servo]][SERVO_MIN_IDX]);
}

int getMin(int servo) {
  return servoData[servoType[servo]][SERVO_MIN_IDX];
}

void setMax(int servo) {
  setValue(servo, servoData[servoType[servo]][SERVO_MAX_IDX]);
}

int getMax(int servo) {
  return servoData[servoType[servo]][SERVO_MAX_IDX];
}

void setValue(int servo, int value) {
  servoPosition[servo] = value;
  //pwm.setPWM( servo, 0, value);
}

