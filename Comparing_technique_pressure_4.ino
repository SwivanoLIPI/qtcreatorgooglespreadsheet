#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP085.h>
#include <EEPROM.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
Adafruit_BMP085 bmp;

unsigned long delayTime;
int addressBME = 0;  // EEPROM address to store BME280 data
int addressBMP = 4;  // EEPROM address to store BMP180 data
double firstCor = 0.0;

double firstPressureBME = 0.0;
double firstPressureBMP = 0.0;
double measuredPressureBMP = 0;
const int numReadings = 10; // Jumlah bacaan untuk moving average
double readingsBME[numReadings]; // Nilai tekanan BME280 yang disimpan
double readingsBMP[numReadings]; // Nilai tekanan BMP180 yang disimpan

int readIndex = 0; // Indeks untuk penyimpanan nilai tekanan

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 and BMP180 test"));
  pinMode(15, OUTPUT);
  
  bool status;

  // Default settings for BME280
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Default settings for BMP180
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }

  delayTime = 10;

  // Inisialisasi nilai awal untuk moving average
  for (int i = 0; i < numReadings; i++) {
    readingsBME[i] = firstPressureBME;
    readingsBMP[i] = firstPressureBMP;
  }

if (firstPressureBME == 0.0) {
    firstPressureBME = bme.readPressure();
    EEPROM.put(addressBME, firstPressureBME);
  }

  if (firstPressureBMP == 0.0) {
    firstPressureBMP = bmp.readPressure();
    EEPROM.put(addressBMP, firstPressureBMP);
  }

  if (firstPressureBMP > firstPressureBME){
    firstCor = firstPressureBMP - firstPressureBME;
  }
  else {
    firstCor = firstPressureBME - firstPressureBMP;
  }
}

void loop() {
  double measuredPressureBME = bme.readPressure();
  if (firstPressureBMP > firstPressureBME){
    
    measuredPressureBMP = bmp.readPressure()- firstCor;
  }
  else{
    
    measuredPressureBMP = bmp.readPressure()- firstCor;
  }
  // Membaca nilai tekanan
  
  

  // Menambahkan nilai tekanan ke array moving average
  readingsBME[readIndex] = measuredPressureBME;
  readingsBMP[readIndex] = measuredPressureBMP;

  // Mengupdate indeks untuk array moving average
  readIndex = (readIndex + 1) % numReadings;

  // Menghitung nilai rata-rata dari array moving average
  double averagePressureBME = 0;
  double averagePressureBMP = 0;

  for (int i = 0; i < numReadings; i++) {
    averagePressureBME += readingsBME[i];
    averagePressureBMP += readingsBMP[i];
  }

  averagePressureBME /= numReadings;
  averagePressureBMP /= numReadings;

  // Menampilkan nilai rata-rata pada Serial Monitor
  printBMEValues(averagePressureBME);
  printBMPValues(averagePressureBMP);

  delay(delayTime);
}

void printBMEValues(double pressure) {
  Serial.print("BME280: ");
  Serial.print(pressure);
  Serial.print(" hPa|");
}

void printBMPValues(double pressure) {
  Serial.print("BMP180: ");
  Serial.print(pressure);
  Serial.print(" hPa");
  Serial.println();
}
