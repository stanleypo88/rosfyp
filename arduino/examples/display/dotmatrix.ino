/*
  Matrix Display Demo
  matrix-display-demo.ino
  Demonstrates features of 4-element 8x8 LED matrix display

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include the required libraries
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Specify display hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Display Size and CS Pin
#define MAX_DEVICES 4
#define CS_PIN 10

// Create a display object
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {

  // Intialize the display object
  myDisplay.begin();

  // Set the intensity (brightness) of the display (0-15)
  myDisplay.setIntensity(5);

  // Clear the display
  myDisplay.displayClear();
}

void loop() {

  // Align left
  myDisplay.setTextAlignment(PA_LEFT);
  myDisplay.print("Left");
  delay(2000);

  // Align center
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.print("Center");
  delay(2000);

  // Align right
  myDisplay.setTextAlignment(PA_RIGHT);
  myDisplay.print("Right");
  delay(2000);

  // Invert display
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.setInvert(true);
  myDisplay.print("Invert");
  delay(2000);

  // Normal text
  myDisplay.setInvert(false);
  myDisplay.print(1234);
  delay(2000);
}
