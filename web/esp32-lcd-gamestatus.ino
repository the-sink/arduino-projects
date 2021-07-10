#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "secret.h" // header file contains SSID and PASSWORD variables for wifi config

HTTPClient http;

const int lcdColumns = 20;
const int lcdRows = 4;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  lcd.init();
  lcd.backlight();
  lcd.print("Connecting...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }
}

String httpGETRequest(const char* serverName) {
  http.begin(serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  
  http.end();
  return payload;
}

void loop(){
  String response = httpGETRequest("https://games.roblox.com/v1/games?universeIds=105407262,173212630");
  JSONVar placeData = JSON.parse(response);
  if (JSON.typeof(placeData) != "undefined"){
    lcd.clear();
    lcd.print("Main: ");
    lcd.print(placeData["data"][0]["visits"]);
    lcd.setCursor(0, 1);
    lcd.print("      ");
    lcd.print(placeData["data"][0]["playing"]);
    lcd.print(" online");
    lcd.setCursor(0, 2);
    lcd.print("Dev:  ");
    lcd.print(placeData["data"][1]["visits"]);
    lcd.setCursor(0, 3);
    lcd.print("      ");
    lcd.print(placeData["data"][1]["playing"]);
    lcd.print(" online");
  }
  delay(60000);
}
