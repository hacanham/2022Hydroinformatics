/*
Assignment 2 - Temp & RH data collection for 24 hours

Measure temperature and RH using sensor3
at a specified scanning and recording interval
write output to file on SD card

Created By:  Haley Canham
Creation Date: 03Feb2022

Sensor Wiring (AM302 Wired DHT22 Sensor):
Red    --> +5V
Black  --> Ground
Yellow --> Digital port 2

SDCard Breakout Wiring:
-----------------------
Breakout CS  --> Arduino Digital Port 10
Breakout DI  --> Arduino Digital Port 11
Breakout DO  --> Arduino Digital Port 12
Breakout CLK --> Arduino Digital Port 13
Breakout GND --> Arduino GND
Breakout 5V  --> Arduino 5V
*/

// Include the DHT sensor library
#include "DHT.h"

// Define the digital pin you are connecting the DHT sensor to
#define DHTPin 2 

// The DHT library supports multiple DHT sensors, so need to 
// define the type of sensor we are using
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Initialize DHT sensor using the pin and sensor type
DHT dht(DHTPin, DHTTYPE);


// Include the SD library
#include <SD.h>


// Declare some needed variables
unsigned long scanInterval = 1;     // Time between scans within the main loop in seconds
unsigned long recordInterval = 10;  // Time between recorded values in seconds
unsigned long currMillis = 0;       // Timing variable
unsigned long prevMillis = 0;       // Timing variable
unsigned long prevRecordMillis = 0; // Timing variable
unsigned long scanCounter = 0;      // A variable to hold a count of scans
unsigned long recordCounter = 0;    // A variable to hold a count of output records
float degF;                         // Variable to hold temperatureF
float degC;                         // Variable to hold temperatureC
float hum;                          // Variable to hold RH


void setup() {
  // Open a serial port so we can watch the output of the sensor
  Serial.begin(9600);
  Serial.println("Initializing the SD card...");
    
  // Initialize the SD Card
  pinMode(10, OUTPUT); // Set the pinMode on digital pin 10 to OUTPUT
  // Attemt to open a connection to the SD card, if it fails, 
  // send a message to the serial port and exit the program
  if (!SD.begin(10)){
    Serial.println("SD card initialization failed!");
    return;
  }
  
  // Initialization of the SD card was successful
  Serial.println("SD card initialization done.");

  // Create a new file on the SD card to which you want to write
  // Only one file can be open at a time
  // If you want to use a different file, close this one first
  File myFile = SD.open("datatest.txt", FILE_WRITE);

  myFile.println("Temperature and Relative Humidity Sensor Output");
  Serial.println("Temperature and Relative Humidity Sensor Output");
  myFile.println("Measurements using a DHT22 Sensor");
  Serial.println("Measurements using a DHT22 Sensor");
  myFile.println("ScanNumber, elapsed time, hum, degF, degC"); 
  Serial.println("ScanNumber, elapsed time, hum, degF, degC");       // Write column headers to output table

  // Close the file 
  myFile.close();

  // Begin communication with the DHT sensor
  dht.begin();

}

void loop() {
  // Get the current time 
  // (number of milliseconds since the program started)
  currMillis = millis();

  // Check timing to see if the scan interval has been reached
  if ((currMillis - prevMillis) >= (scanInterval * 1000))
  {
    // If YES, do a scan
    // -----------------  
    // Increment the scan counter variable
    scanCounter ++;  // The "++" operator adds 1 to a variable

    // Perform measurements and calculations here
    // ------------------------------------------
  
    // Get the most recent measurements from the sensor
    float hum = dht.readHumidity();  // Get the humidity value in percent
    float degC = dht.readTemperature(); // Get the temperature value in Celisius

    // The sensor will also return the temperature as Fahrenheit (isFahrenheit = true)
    float degF = dht.readTemperature(true);

    // Check to see if any reads failed and exit the loop early to try again
    // "||" is the Arduino Boolean "OR" operator
    if (isnan(hum) || isnan(degC) || isnan(degF)) {
      Serial.println("Failed to read from DHT sensor!");
      return; // This exits the main loop and starts the next iteration
    }

    // Manage the timing variables to reflect that I just finished a scan
    prevMillis = currMillis;

    // Check to see if it's time to record data
    if ((currMillis - prevRecordMillis) >= (recordInterval * 1000))
    {
      // Record a data record - for now print to the serial port 
      // A later example will show how to write to a file
      // ---------------------------------------------------------------

      // You can also write data to the file within the main loop
      // For example if you want to write out the most recent observations
      // for a recording interval. 
  
      // Open the file again
      File myFile = SD.open("datatest.txt", FILE_WRITE);

      
      // Print the sensor output to the serial port
      String dataRecord = String(recordCounter) + ","+ String(currMillis)+","+String(hum)+","+String(degF)+","+String(degC);

      myFile.println(dataRecord);
      Serial.println(dataRecord);

      // Close the file
      myFile.close();

      // Increment the record counter
      recordCounter ++;
      
      // Manage to timing variables to reflect that I just recorded data
      // and reset the scanCounter and randomSum variables for the next scan
      prevRecordMillis = currMillis;
      scanCounter = 0;
    }
  }
}
