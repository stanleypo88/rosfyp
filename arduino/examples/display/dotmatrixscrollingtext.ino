/*
  Matrix Display Scrolling Demo
  matrix-scroll-demo.ino
  Scrolls text on a 4-element 8x8 LED matrix display

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

  // Display our text in a scroll
  myDisplay.displayScroll("Welcome to the Workshop!", PA_CENTER, PA_SCROLL_LEFT, 100);
}

void loop() {

  // Ensure display stays animated
  if (myDisplay.displayAnimate()) {
    myDisplay.displayReset();
  }
}
