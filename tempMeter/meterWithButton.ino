// This is a copy of meterCycle.ino but instead of the readings switching every 5 seconds, it samples both values every 2 seconds and has a button wired to pin D3.
// This button pulls the pin low, triggering an interrupt and switching the display between temperature and humidity (humidity = dot indicator visible).

#include <ShiftOutX.h>
#include <ShiftPinNo.h>
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22

shiftOutX disp(9, 8, 10, MSBFIRST, 2);
DHT dht(DHTPIN, DHTTYPE);

// register pin numbers for displays
const int dispOne[] = {shPin2, shPin3, shPin4, shPin5, shPin6, shPin7, shPin8};
const int dispTwo[] = {shPin10, shPin11, shPin12, shPin9, shPin14, shPin16, shPin15};

const int digits[10][7]{
  {1, 1, 1, 0, 1, 1, 1},
  {0, 0, 1, 0, 0, 0, 1},
  {1, 1, 0, 1, 0, 1, 1},
  {0, 1, 1, 1, 0, 1, 1},
  {0, 0, 1, 1, 1, 0, 1},
  {0, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1},
  {0, 1, 1, 1, 1, 1, 1},
};

String a;
bool showHum = false;

int temp = 0;
int hum = 0;

void display_digit(int dispSegment[], int d){
  for (int i = 0; i < 7; i++){
    if (digits[d][i] == 0){
      disp.pinOn(dispSegment[i]);
    } else {
      disp.pinOff(dispSegment[i]);
    }
  }
}

void display_number(int n, bool dot){
  int ten = (int)(n / 10);
  int one = n - (ten * 10);

  display_digit(dispOne, ten);
  display_digit(dispTwo, one);

  if (dot) {
    disp.pinOff(shPin13);
  } else {
    disp.pinOn(shPin13);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), changeMode, FALLING);
  dht.begin();
  
  disp.allOff();
  delay(1000);
  disp.allOn();
}

void changeMode() {
  showHum = !showHum;
}

void loop() {
  temp = constrain(round(dht.readTemperature() * 1.8 + 32), 0, 99);
  hum = constrain(round(dht.readHumidity()), 0, 99);
  for (int i = 0; i <= 7; i++) {
    if (showHum) {
      display_number(hum, true);
    } else {
      display_number(temp, false);
    }
    delay(250);
  }
}
