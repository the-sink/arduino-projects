#include <ShiftOutX.h>
#include <ShiftPinNo.h>
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22

shiftOutX regOne(9, 8, 10, MSBFIRST, 2);
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

void display_digit(int disp[], int d){
  for (int i = 0; i < 7; i++){
    if (digits[d][i] == 0){
      regOne.pinOn(disp[i]);
    } else {
      regOne.pinOff(disp[i]);
    }
  }
}

void display_number(int n, bool dot){
  int ten = (int)(n / 10);
  int one = n - (ten * 10);

  display_digit(dispOne, ten);
  display_digit(dispTwo, one);

  if (dot) {
    regOne.pinOff(shPin13);
  } else {
    regOne.pinOn(shPin13);
  }
}

void setup() {
  dht.begin();
  regOne.allOff();
  delay(1000);
  regOne.allOn();
}

void loop() {
  int temp = constrain(round(dht.readTemperature() * 1.8 + 32), 0, 99);
  int hum = constrain(round(dht.readHumidity()), 0, 99);
  
  display_number(temp, false);
  delay(5000);
  display_number(hum, true);
  delay(5000);
}
