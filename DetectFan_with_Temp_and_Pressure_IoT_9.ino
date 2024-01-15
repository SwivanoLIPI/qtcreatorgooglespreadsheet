#include "TRIGGER_WIFI.h"               /*Includes ESP8266WiFi.h and WiFiClientSecure.h, just have these two libraries downloaded before*/
#include "TRIGGER_GOOGLESHEETS.h"       /*Library file for Google Sheets, has to be used after Wi-Fi Client Secure declaration, here everything is in Trigger_WIFI.h, so using it after Trigger_WIFI.h*/ 
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/**********Google Sheets Definations***********/
#define SEALEVELPRESSURE_HPA (1013.25)
#define FAN_PIN 15
#define RELAY_DELAY 10000 // 10 seconds
char column_name_in_sheets[ ][20] = {"value1","value2","value3"};                        /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbzFHe7E32oQpn6HLP86HMzWXpGiieDv9NHUbffKmqdlffi3EOgHvoq55qW3yjLMNZXw";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/
Adafruit_BME280 bme;
float c = 0;
int address = 0; // EEPROM address to store data

double firstTemperature = 0.0;
unsigned long previousMillis = 0;
int addressMillis = sizeof(double); // EEPROM address to store previousMillis

void setup(){
  Serial.begin(9600);       
  Serial.println(F("BME280 test"));
  pinMode(FAN_PIN, OUTPUT);
  bool status;
  status = bme.begin(0x76);

  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  if (firstTemperature == 0.0)
  {
    // Save the first value to EEPROM
    firstTemperature = bme.readTemperature();
    previousMillis = millis();
    EEPROM.put(address, firstTemperature);
    EEPROM.put(addressMillis, previousMillis);
  }
  while (!Serial);
  //WIFI_Connect("Agmalia","090917_SA");
  WIFI_Connect("Porlak","Heto2023"); /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );         /*Sets the column name for Google Sheets, the GAS ID, and the No of Parameter we want to send*/ 

}

void loop() 
{
   double currentTemperature = bme.readTemperature();
   float CT = currentTemperature;
   float CP = bme.readPressure();
   float FT = firstTemperature;
   Serial.print(currentTemperature);
   Serial.print(" *C|");
   Serial.print(previousMillis);
   Serial.print("ms|");
   Serial.print(firstTemperature);
   Serial.print("*C|");
   
  // Cek suhu setiap detik
   unsigned long currentMillis = millis();
   Serial.print(currentMillis);
   Serial.print("ms");
  if (currentMillis - previousMillis >= 30000) {
   
    if (currentTemperature < firstTemperature - 0.5) {
   
    digitalWrite(FAN_PIN, LOW);
    Serial.println("| Fan OFF");
   // c = "Fan OFF";
    Data_to_Sheets(No_of_Parameters,  CP,  CT, 0.00);
  } else {
    
      digitalWrite(FAN_PIN, HIGH);
      Serial.println("| Fan ON");
     // c = "Fan ON";
      Data_to_Sheets(No_of_Parameters,  CP,  CT, 1.00);
  }
    
  }
  else {
    Serial.println("|Wait");
  }
  
  
  Serial.println();
  delay(1000);       
}
