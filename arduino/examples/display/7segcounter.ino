/*
  7-Segment Counter - Single Digit, directly wired
  7-seg-single-count.ino
  Count to a single 7-segment display

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

  // Count from 0 to 9
  for (int i = 0; i < 10; i++) {
    // Set number for display
    sevseg.setNumber(i);
    // Wait one second
    delay(1000);
    // Refresh the display
    sevseg.refreshDisplay();
  }
}
