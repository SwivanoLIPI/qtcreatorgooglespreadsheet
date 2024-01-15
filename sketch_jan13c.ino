#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define ON_Board_LED 2

const char* ssid = "S 1707";
const char* password = "12345678";
const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;
String GAS_ID = "AKfycbxYt46cxWfx7b3WbwG1oLv7jiiawsXFmj1IWZsFyxmtpS9Sq8ges539bobVK-6tkweF"; // spreadsheet script ID

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi. Please check your credentials or reset the ESP32.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  connectToWiFi();

  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH);

  client.setInsecure();
}

void loop() {
  sensors_event_t event;
  bmp.getEvent(&event);

  if (isnan(event.temperature)) {
    Serial.println("Failed to read temperature from BMP180 sensor !");
    delay(5000);
    return;
  }

  float temperature;
  bmp.getTemperature(&temperature);

  String tempString = "Temperature : " + String(temperature) + " °C";
  Serial.println(tempString);

  sendData(temperature);
  delay(5000); // Adjust the delay according to your requirements
}

void sendData(float temperature) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    connectToWiFi();  // Coba menghubungkan ulang ke Wi-Fi
    return;
  }

  String string_temperature = String(temperature);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&Fan=";
  
  if (temperature > 30) {
    url += "On";
  } else {
    url += "Off";
  }

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: ESP32\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("ESP32/Arduino CI success!");
  } else {
    Serial.println("ESP32/Arduino CI failed");
  }

  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  
  client.stop();  // Bebaskan koneksi setelah pengiriman data
}
