/*
  Stepper Motor variables for DRV8825
  https://www.pololu.com/product/2133

  Microstepping pinout
  M0  M1  M2  Microstep
  0   0   0   Full
  1   0   0   1/2
  0   1   0   1/4
  1   1   0   1/8
  0   0   1   1/16
  1   0   1   1/32
  0   1   1   1/32
  1   1   1   1/32
*/

#define stpStepPin 53
#define stpDirPin 52
#define stpM0Pin 51
#define stpM1Pin 50
#define stpM2Pin 49
//#define stpFaultPin 48  //Pin left NC
//#define stpReset 47  //Pin left NC

volatile long stpStepCount = 0;
bool stpDir = 0;  //0: CW  1: CCW

/*
  Horizontal Soft Potentiometer Feedback
  http://www.mouser.com/ProductDetail/Spectra-Symbol/SP-L-0750-203-3-ST/?qs=sGAEpiMZZMvWgbUE6GM3Oc6i9sqQyBsUI0fLnd5p%2fsI%3d
*/

#define horPotPin A0

int incomingByte = 0;

void setup() {

  Serial.begin(115200);

  //Horizontal Stepper Motor
  pinMode(stpStepPin, OUTPUT);
  pinMode(stpDirPin, OUTPUT);
  pinMode(stpM0Pin, OUTPUT);
  pinMode(stpM1Pin, OUTPUT);
  pinMode(stpM2Pin, OUTPUT);
  //pinMode(stpFaultPin, INPUT_PULLUP);   //Used for software detection of overtemp or overcurrent

  //Set stepper driver microstep mode
  //1/4 step mode
  digitalWrite(stpM0Pin, LOW);
  digitalWrite(stpM1Pin, LOW);
  digitalWrite(stpM2Pin, LOW);

  //Horizontal Potentiometer Pin
  pinMode(horPotPin, INPUT);

}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (incomingByte == 114) { //r
      backAndForth();
    } else if (incomingByte == 115) {

    }
  }

  Serial.println(analogRead(horPotPin));
}

/*
  Moves the stepper motor a number of steps in the specified direction
*/
void stpStep(int steps, bool dir) {

  if (dir) {
    digitalWrite(stpDirPin, HIGH);
  } else {
    digitalWrite(stpDirPin, LOW);
  }

  if (steps > 0) {

    for (int i = 0; i < steps; i++) {
      digitalWrite(stpStepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stpStepPin, LOW);
      delayMicroseconds(500);
    }
  } else {
    Serial.println("Invalid number of steps");
  }
}

void backAndForth() {
  while (!(Serial.available() > 0)) {
    while (analogRead(horPotPin) < 915) {
      digitalWrite(stpDirPin, HIGH);
      digitalWrite(stpStepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stpStepPin, LOW);
      delayMicroseconds(500);
    }

    while (analogRead(horPotPin) > 100) {
      digitalWrite(stpDirPin, LOW);
      digitalWrite(stpStepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stpStepPin, LOW);
      delayMicroseconds(500);
    }
  }
}

