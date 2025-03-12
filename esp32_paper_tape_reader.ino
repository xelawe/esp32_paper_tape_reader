/*
    Manual Paper Tape Reader with USB Serial (CDC) Interface
    Based on: https://www.e-basteln.de/file/papertape/Paper_Tape_Reader.ino

    adapted for ESP32
    reads 5bits
*/


#include <Baudot.h>

BAUDOT Baudot;
int CODE;

#define max_pins 6
//int pins[] = {4, 1, 2, 3, 5, 6 };
int pins[] = {4, 6, 5, 3, 2, 1 };
int th_val[] = { 8000, 7000, 7000, 7000, 7000, 7000 };
int adc_val[8];
byte dig_val[8];
byte old_index = 0;
byte debug = 0;

#define pin_debug 39
#define pin_debug1 37

void setup() {

  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);

  pinMode(pin_debug, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Starting ..");

  for (int i = 0; i < max_pins; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }

}

void loop() {

  // read index
  adc_val[0] = analogRead(pins[0]);
  if (adc_val[0] < th_val[0] ) {
    dig_val[0] = 1;
  } else {
    dig_val[0] = 0;
  }

  debug = digitalRead(pin_debug);

  // index entered hole?
  if ( debug == 1 || (old_index == 0 && dig_val[0] == 1 ) ) {
    old_index = 1;

    digitalWrite(15, HIGH); // LED indicates scanning

    CODE = 0;
    for (int i = 1; i < max_pins; i++) {
      adc_val[i] = analogRead(pins[i]);
      if (adc_val[i] < th_val[i] ) {
        dig_val[i] = 1;
      } else {
        dig_val[i] = 0;
      }
      CODE = CODE + dig_val[i] * pow(2, i - 1);
    }

    digitalWrite(15, LOW);

    if ( debug == 1 ) {
      //Serial.print("=> ");
      for (int i = 0; i < max_pins; i++) {

        Serial.print(dig_val[i]);
        if (i == 0) {
          Serial.print(" ");
        }
      }
      Serial.print(" ");
      Serial.print(CODE);
      Serial.print(" ");
      Serial.print(Baudot.Decode(CODE));
      Serial.print(" <= ");
      for (int i = 0; i < max_pins; i++) {
        Serial.print(adc_val[i]);
        Serial.print(" ");
      }
      Serial.println();
    } else {
      Serial.print(Baudot.Decode(CODE));
    }
  } else if (old_index == 1 && dig_val[0] == 0 ) {
    // index left hole
    old_index = 0;
    delay(10);
  } else {
    delay(10);
  }



}
