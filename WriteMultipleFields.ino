/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: Arduino MKR1000
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires WiFi101 library version 0.15.3 or newer.
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <WiFiNINA.h>
#include <DHT.h>
#include "secrets.h"
#include "ThingSpeak.h"

#define DHTPIN 2       // Pin connected to the DHT sensor
#define DHTTYPE DHT11  // Type of DHT sensor

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);      // Initialize serial 

  dht.begin();  // Initialize DHT sensor
  ThingSpeak.begin(client);  // Initialize ThingSpeak 
}

void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if reading is valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Update ThingSpeak channel with sensor data
  ThingSpeak.setField(1, temperature); // Field 1 for temperature
  ThingSpeak.setField(2, humidity);    // Field 2 for humidity

  // Write to ThingSpeak
  int updateStatus = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (updateStatus == 200) {
    Serial.println("Channel update successful!");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(updateStatus));
  }

  delay(60000); // Update every 60 seconds
}


