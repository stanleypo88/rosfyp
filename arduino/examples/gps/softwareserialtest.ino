/*
  SoftwareSerial Echo
  software-serial-echo.ino
  Echos data from SoftwareSerial
  Use to display GPS raw data
  Results on serial monitor

  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/
 
 // Define SoftwareSerial Connection   
#define swsTX 3 // Transmit FROM GPS
#define swsRX 4 // Receive TO GPS

//GPS Baud rate
#define GPSBaud 9600 

//Serial Monitor Baud Rate
#define Serial_Monitor_Baud 115200   

// Include and set up the SoftwareSerial Library
#include <SoftwareSerial.h> 
SoftwareSerial GPSserial(swsRX, swsTX);  

void setup()
{
 //Start Serial Monitor
 Serial.begin(Serial_Monitor_Baud); 
 
 // Start SoftwareSerial  
 GPSserial.begin(GPSBaud);
}
    
void loop()
{
  // Write SoftwareSerial data to Serial Monitor
  while (GPSserial.available() > 0)
  Serial.write(GPSserial.read());
}
