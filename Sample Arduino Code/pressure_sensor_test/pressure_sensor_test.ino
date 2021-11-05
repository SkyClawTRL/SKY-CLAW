int leftPressurePin = A1;
int rightPressurePin = A2;

void setup() {
  Serial.begin(9600);
  pinMode(leftPressurePin, INPUT);
  pinMode(rightPressurePin, INPUT);
}

void loop() {
  Serial.print(analogRead(leftPressurePin));
  Serial.print(",");
  Serial.println(analogRead(rightPressurePin));
  delay(10);

}
