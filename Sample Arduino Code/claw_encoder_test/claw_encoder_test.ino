#include <Encoder.h>

Encoder myEnc(2, 3);

void setup() {
  Serial.begin(9600);
}

long oldPosition  = -999;

void loop() {

  Serial.println(myEnc.read());
  delay(10);
}
