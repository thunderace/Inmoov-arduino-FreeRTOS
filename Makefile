BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyUSB1
ARDUINO_LIBS = Adafruit-PWM-Servo-Driver-Library Adafruit_NeoPixel Arduino_FreeRTOS Wire
ARDUINO_SKETCHBOOK = $(HOME)/arduino
ARDUINO_DIR = $(HOME)/arduino
TEMP_DIR = $(HOME)/temp
USER_LIB_PATH=$(CURDIR)/libraries
PROJECT_NAME = Domo1
MONITOR_CMD=minicom
MONITOR_BAUDRATE=115200
include $(HOME)/Arduino-Makefile-sudar/Arduino.mk