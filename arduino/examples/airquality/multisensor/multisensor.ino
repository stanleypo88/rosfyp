/*
  ESP32 Air Quality Measurement
  air-quality.ino
  Measures air quality with multiple sensors
  Uses ESP32 Module (must have GPIO 21 & 22 available)
  Preferences library used to store SGP30 baseline values
  Adafruit PM2.5 sensor with UART
  Adafruit SGP30 Air Quality Sensor
  DFRobot BM680 Temp & Humid & Air Pressure Module
  Adafruit AHT 20 module
  DFRobot SGP40 Air Quality Sensor

  PM2.5 functions from howtoelectronics.com - https://how2electronics.com/interfacing-pms5003-air-quality-sensor-arduino/

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include "Wire.h"
#include "Adafruit_SGP30.h"
#include "DFRobot_BME680_I2C.h"
#include "Adafruit_AHTX0.h"
#include "DFRobot_SGP40.h"
#include "Preferences.h"

// Serial Port connections for PM2.5 Sensor
#define RXD2 16 // To sensor TXD
#define TXD2 17 // To sensor RXD

// Altitude calibration for BME680
#define CALIBRATE_PRESSURE

// Object for BME680 Sensor at I2C address 0x76
DFRobot_BME680_I2C bme(0x76);

// Object for SGP30 Sensor
Adafruit_SGP30 sgp30;

// Object for SGP40 Sensor
DFRobot_SGP40 sgp40;

// Object for AHT20 Sensor
Adafruit_AHTX0 aht;

// Objects for AHT20 Humidity & Temperature
sensors_event_t aht20_humidity, aht20_temp;

// Preferences object for storing non-volatile data
Preferences preferences;

// Variable for VOC Index
uint16_t VOCindex ;

// Temperature variable
float local_temperature;

// Humidity variable
float local_humidity;

// Local altitude in meters for BME680 (adjust to match your location)
float altLocal = 33.06;

// Sea Level variable
float seaLevel;

// Counter for SGP30 Sensor recalibration
int sgp30counter = 0;

// Baseline variables for SG30
uint16_t TVOC_base;
uint16_t eCO2_base;

// Function to return absolute humidity [mg/m^3] with approximation formula
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

// Data structure for PM2.5 sensor data
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

// Create data object for PMm2.5 sensor data
struct pms5003data data;

// Function to read PM2.5 sensor data
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // Get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian, this resolves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // Put it into a nice structure
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // Success!
  return true;
}

void setup()
{
  // Start serial monitor port
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Starting sensors");

  // Open namespace for read and write
  preferences.begin("param", false);

  // Wait to allow sensors to get ready
  delay(2000);

  // Set up UART connection
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Result for BME680 connection
  uint8_t rslt = 1;

  // Start BME680
  while (rslt != 0) {
    rslt = bme.begin();
    if (rslt != 0) {
      Serial.println("BME680 sensor not found :(");
      delay(2000);
    }
  }

  Serial.println("BME680 started successfully");
#ifdef CALIBRATE_PRESSURE
  bme.startConvert();
  delay(1000);
  bme.update();
  seaLevel = bme.readSeaLevel(altLocal);
  Serial.print("BME680 Sea Level :");
  Serial.println(seaLevel);
#endif

  // Start AHT20 Sensor
  if (! aht.begin()) {
    Serial.println("AHT20 sensor not found :(");
    while (1) delay(10);
  }

  Serial.println("AHT20 started successfully");

  // Start SGP30 Sensor
  if (! sgp30.begin()) {
    Serial.println("SGP30 sensor not found :(");
    while (1);
  }

  Serial.println("SGP30 started successfully");

  // Set baseline for SGP30 sensor from stored values
  // Use default value if no data stored
  TVOC_base = preferences.getUInt("tvoc", 0);
  eCO2_base = preferences.getUInt("eco2", 0);
  sgp30.setIAQBaseline(eCO2_base, TVOC_base);

  // Start SGP40 Sensor
  while (sgp40.begin(10000) != true) {
    Serial.println("SGP40 sensor not found :(");
    delay(1000);
  }

  Serial.println("SGP40 started successfully");

  Serial.println("All sensors initialized, 5-second warm-up delay before first reading");
  delay(5000);

}

void loop()
{

  // Get AHT20 Data
  aht.getEvent(&aht20_humidity, &aht20_temp);

  // Read temperature and humidity from AHT20
  local_temperature = aht20_temp.temperature;
  local_humidity = aht20_humidity.relative_humidity;

  // Get BME680 data
  bme.startConvert();
  bme.update();

  // Compensate SGP30 for temperature and humidity
  sgp30.setHumidity(getAbsoluteHumidity(local_temperature, local_humidity));

  // Get SGP30 data
  if (! sgp30.IAQmeasure()) {
    Serial.println("SGP30 Measurement failed");
    return;
  }

  // SGP30 Raw data
  if (! sgp30.IAQmeasureRaw()) {
    Serial.println("SGP30 Raw Measurement failed");
    return;
  }

  // Increment SGP30 Counter, take baseline every 30 readings
  sgp30counter++;
  if (sgp30counter == 30) {
    sgp30counter = 0;
    // Get new baseline readings
    uint16_t TVOC_base_new, eCO2_base_new;
    if (! sgp30.getIAQBaseline(&eCO2_base_new, &TVOC_base_new)) {
      Serial.println("Failed to get SGP30 baseline readings");
      return;
    }
    // Update stored baseline values if they are different than the existing ones
    if (TVOC_base_new != TVOC_base || eCO2_base_new != eCO2_base) {
      TVOC_base = TVOC_base_new;
      eCO2_base = eCO2_base_new;
      // Update EEPROM
      preferences.putUInt("tvoc", TVOC_base);
      preferences.putUInt("eco2", eCO2_base);
    }

  }

  // Compensate SGP40 for temperature and humidity
  sgp40.setRhT( local_humidity, local_temperature);

  // Get VOC Index readings from SGP40
  VOCindex = sgp40.getVoclndex();

  // Print results to serial monitor if PM2.5 sensor is ready
  if (readPMSdata(&Serial1)) {
    Serial.println();
    Serial.println(F("-------------------------------------------------"));
    // SGP30 Data
    Serial.print(F("TVOC: \t\t\t\t")); Serial.print(sgp30.TVOC); Serial.println(" ppb");
    Serial.print(F("eCO2: \t\t\t\t")); Serial.print(sgp30.eCO2); Serial.println(" ppm");
    Serial.print(F("Raw H2: \t\t\t")); Serial.print(sgp30.rawH2); Serial.println(" ppb");
    Serial.print(F("Raw Ethanol: \t\t\t")); Serial.print(sgp30.rawEthanol); Serial.println(" ppb");
    // SG40 Data
    Serial.print(F("VOC Index: \t\t\t")); Serial.println(VOCindex);
    // AHT20 data
    Serial.print(F("Temperature: \t\t\t")); Serial.print(aht20_temp.temperature, 2); Serial.println(" C");
    Serial.print(F("Humidity: \t\t\t"));  Serial.print(aht20_humidity.relative_humidity); Serial.println(" %rh");
    // BME680 Data
    Serial.print(F("Air Pressure: \t\t\t"));  Serial.print(bme.readPressure() / 1000, 2); Serial.println(" kPa");
    Serial.print(F("Sensor Altitude: \t\t"));  Serial.print(bme.readAltitude()); Serial.println(" m");
#ifdef CALIBRATE_PRESSURE
    Serial.print(F("Calibrated Altitude: \t\t"));  Serial.print(bme.readCalibratedAltitude(seaLevel)); Serial.println(" m");
#endif
    // PM2.5 Data
    Serial.print(F("Standard PM 1.0: \t\t")); Serial.println(data.pm10_standard);
    Serial.print(F("Standard PM 2.5: \t\t")); Serial.println(data.pm25_standard);
    Serial.print(F("Standard PM 10: \t\t")); Serial.println(data.pm100_standard);
    Serial.print(F("Environmental PM 1.0: \t\t")); Serial.println(data.pm10_env);
    Serial.print(F("Environmental PM 2.5: \t\t")); Serial.println(data.pm25_env);
    Serial.print(F("Environmental PM 10: \t\t")); Serial.println(data.pm100_env);
    Serial.print(F("Particles > 0.3um / 0.1L air: \t")); Serial.println(data.particles_03um);
    Serial.print(F("Particles > 0.5um / 0.1L air: \t")); Serial.println(data.particles_05um);
    Serial.print(F("Particles > 1.0um / 0.1L air: \t")); Serial.println(data.particles_10um);
    Serial.print(F("Particles > 2.5um / 0.1L air: \t")); Serial.println(data.particles_25um);
    Serial.print(F("Particles > 5.0um / 0.1L air: \t")); Serial.println(data.particles_50um);
    Serial.print(F("Particles > 10 um / 0.1L air: \t")); Serial.println(data.particles_100um);
    // SGP30 Baseline values
    Serial.print(F("SGP30 Baseline (HEX) - eCO2: \t")); Serial.println(eCO2_base, HEX);
    Serial.print(F("SGP30 Baseline (HEX) - TVOC: \t")); Serial.println(TVOC_base, HEX);

  }

}
