#include <Encoder.h>

Encoder myEnc(2, 3);

// DC motor pin definitions
const uint8_t inAPin = 7;
const uint8_t inBPin = 8;
const uint8_t dcSpeedPin = 9;

int target = 0;
int error = 0;

void setup() {
  Serial.begin(9600);
  pinMode(dcSpeedPin, OUTPUT);
  pinMode(inAPin, OUTPUT);
  pinMode(inBPin, OUTPUT);

}

long oldPosition  = -999;

void loop() {
  
  target = 2000;
  error = target - myEnc.read();
  while(abs(error) > 50){
    error = target - myEnc.read();
    if(error > 0){
      digitalWrite(inAPin, HIGH);
      digitalWrite(inBPin, LOW);
      analogWrite(dcSpeedPin, 235);
    }
    if(error < 0){
      digitalWrite(inAPin, LOW);
      digitalWrite(inBPin, HIGH);
      analogWrite(dcSpeedPin, 235);
    }
    Serial.print(myEnc.read());
    Serial.print(",");
    Serial.println(target);
  }
  analogWrite(dcSpeedPin, 0);

  target = 0;
  error = target - myEnc.read();
  while(abs(error) > 50){
    error = target - myEnc.read();
    if(error > 0){
      digitalWrite(inAPin, HIGH);
      digitalWrite(inBPin, LOW);
      analogWrite(dcSpeedPin, 235);
    }
    if(error < 0){
      digitalWrite(inAPin, LOW);
      digitalWrite(inBPin, HIGH);
      analogWrite(dcSpeedPin, 235);
    }
    Serial.print(myEnc.read());
    Serial.print(",");
    Serial.println(target);
  }
  analogWrite(dcSpeedPin, 0);

}
