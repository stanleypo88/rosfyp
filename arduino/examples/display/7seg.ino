/*
  7-Segment Display - Single Digit, directly wired
  7-seg-single-digit.ino
  Send a number to a single 7-segment display

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include library
#include "SevSeg.h"

// Create object
SevSeg sevseg;

// Number of digits in display
byte numDigits = 1;
// Display select pins (not used with single display)
byte digitPins[] = {};
// Display segment pins A,B,C,D,E,F,G,DP
byte segmentPins[] = {6, 5, 2, 3, 4, 7, 8, 9};
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

  // Set number to display
  sevseg.setNumber(3);
  // Refresh the display
  sevseg.refreshDisplay();
}
