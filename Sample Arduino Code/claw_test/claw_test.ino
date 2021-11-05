#include <SPI.h>
#include <AMIS30543.h>
#include <AccelStepper.h>

// Stepper motor pin definitions
const uint8_t amisDirPin = 2; //D-sub pin: 7
const uint8_t amisStepPin = 3; //D-sub pin: 6
const uint8_t amisSlaveSelect = 4; //D-sub pin: 10

// DC motor pin definitions
const uint8_t dcSpeedPin = 5;
const uint8_t inAPin = 6;
const uint8_t inBPin = 7;



AMIS30543 stepper;
AccelStepper accelStepper(AccelStepper::DRIVER, amisStepPin, amisDirPin);


void setup() {
  SPI.begin();
  stepper.init(amisSlaveSelect);
  delay(1);

  stepper.resetSettings();
  stepper.setCurrentMilliamps(1000);
  stepper.setStepMode(32);
  stepper.enableDriver();

  accelStepper.setMaxSpeed(2000.0);
  accelStepper.setAcceleration(500.0);

}

void loop() {

  accelStepper.runToNewPosition(0);
  delay(500);
  accelStepper.runToNewPosition(2000);
  delay(500);

}
