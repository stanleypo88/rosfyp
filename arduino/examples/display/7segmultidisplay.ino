/*
  7-Segment Display - Four Digit, directly wired
  7-seg-multi.ino
  Print a number to a 4-digit 7-segment display

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include library
#include "SevSeg.h"

// Create object
SevSeg sevseg;

// Number of digits in display
byte numDigits = 4;
// Display select pins
byte digitPins[] = {10, 11, 12, 13};
// Display segment pins A,B,C,D,E,F,G,DP
byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
// Dropping resistors used
bool resistorsOnSegments = true;
// Display type
byte hardwareConfig = COMMON_CATHODE;

void setup() {

  // Start display object
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  // Set brightness
  sevseg.setBrightness(90);
}

void loop() {

  // Set the number to display
  sevseg.setNumber(1234);
  // Refresh the display
  sevseg.refreshDisplay();
}
