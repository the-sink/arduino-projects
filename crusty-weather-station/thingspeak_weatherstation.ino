#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTTYPE DHT22

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 300

/*
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_WRONG_PASSWORD   = 6,
    WL_DISCONNECTED     = 7
*/

const char* ssid = "lol";   // your network SSID (name)
const char* password = "lol";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "lol";

bool firstBoot = true;

int phase = 0;

unsigned long millisAtStart = 0;

// Create a sensor object
Adafruit_BMP085 bmp;
DHT dht(16, DHTTYPE);

void initBMP() {
  Serial.println("Starting sensors");
  dht.begin();
  if (!bmp.begin(0x77)) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }
}

void errorBlink() {
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
}

void normalBlink() {
  delay(100);
  digitalWrite(2, LOW);
}

void sendTemperature() {
  float temperatureF = dht.readTemperature(true) - 1;
  if (isnan(temperatureF)) {
    temperatureF = bmp.readTemperature() * 1.8 + 32 - 0.999;
    Serial.println("DHT22 read error - falling back to BMP180 for temperature measurement");
  }
  Serial.print("Temperature (ºF): ");
  Serial.println(temperatureF);

  int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
    normalBlink();
  }
  else {
    Serial.println("Problem updating temperature. HTTP error code " + String(x));
    errorBlink();
  }
}

void sendHumidity() {
  float humidity = dht.readHumidity();
  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  int x = ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
    normalBlink();
  }
  else {
    Serial.println("Problem updating temperature. HTTP error code " + String(x));
    errorBlink();
  }
}

void sendPressure() {
  float pressure = bmp.readPressure() / 100;
  Serial.print("Pressure (hPa): ");
  Serial.println(pressure);

  int x = ThingSpeak.writeField(myChannelNumber, 3, pressure, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
    normalBlink();
  }
  else {
    Serial.println("Problem updating pressure. HTTP error code " + String(x));
    errorBlink();
  }
}

void sendVBat() {
  float bat1 = analogRead(34);
  delay(100);
  float bat2 = analogRead(34);
  delay(100);
  float bat3 = analogRead(34);
  delay(100);
  float bat4 = analogRead(34);
  delay(100);
  float bat5 = analogRead(34);
  float avg = (bat1 + bat2 + bat3 + bat4 + bat5) / 5;
  Serial.print("Battery: ");
  Serial.println(avg);

  int x = ThingSpeak.writeField(myChannelNumber, 4, avg, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
    normalBlink();
  }
  else {
    Serial.println("Problem updating pressure. HTTP error code " + String(x));
    errorBlink();
  }
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  Serial.println("Startup");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //btStop();

  initBMP();

  pinMode(2, OUTPUT);
  pinMode(34, INPUT);
  pinMode(25, INPUT);

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.println("ThingSpeak initialized");
  WiFi.mode(WIFI_STA);

  millisAtStart = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    WiFi.begin(ssid, password);
    delay(100);
    while (WiFi.status() != WL_CONNECTED) {
      int wifiStatus = WiFi.waitForConnectResult();
      if (wifiStatus == 3) {
        continue;
      }
      Serial.print("\nStatus ");
      Serial.print(wifiStatus);
      Serial.println(", retrying...");
      WiFi.disconnect();
      delay(100);
      WiFi.begin(ssid, password);
      delay(200);
    }
    Serial.println("\nConnected.");
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }

  sendVBat();
  delay(15000);
  sendTemperature();
  delay(15000);
  sendHumidity();
  delay(15000);
  sendPressure();

  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {

}
