/*
  Sky Claw Test
  Author: Ben Bradley
  Date: 02/01/2018

  Version:
  1. Stepper Motor Control
  2. Vertical Motor and Encoder Control
  3. Horizontal Potentiometer readings
*/

/*
  Stepper Motor variables for DRV8825
  https://www.pololu.com/product/2133

  Microstepping pinout
  M0  M1  M2  Microstep
  0   0   0   Full
  1   0   0   1/2
  0   1   0   1/4
  1   1   0   1/8
  0   0 	1   1/16
  1   0 	1   1/32
  0   1 	1   1/32
  1   1 	1   1/32
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

/*
  Vertical Motor Variables for L298N wired in parallel
  http://www.bristolwatch.com/L298N/index.htm

  In1 high In2 low: CW DOWN In1 low In2 high: CCW UP
*/
#define vertMotIn1 46   //pin 46
#define vertMotIn2 45   //pin 45
#define vertMotEn 5    // PWM input for motor PE3

bool vertMotDir = 0;  //0: CW down 1: CCW up
bool * dcnt = &vertMotDir;

/*
  Vertical Encoder Variables
  https://www.sparkfun.com/products/10932
*/
#define vertEnA 3
#define vertEnB 2

volatile long vertCnt = 0;
long * cpnt = &vertCnt;
long target = 38401;
long * tpnt = &target;

int vertTravCnt = 0;
int vertRevCnt = 400;  //Counts per rev



/*
  Vertical Limit Switches
*/
#define limSw1 42
#define limSw2 41

//Register manipulation macros
#define SETBIT(REG, nbit) (REG |= (1 << nbit))
#define TOGGLEBIT(REG, nbit) (REG ^= (1 << nbit))
#define CLEARBIT(REG, nbit) (REG &= ~(1 << nbit))



int incomingByte = 0;

void setup() {

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
  digitalWrite(stpM1Pin, HIGH);
  digitalWrite(stpM2Pin, LOW);

  //Horizontal Potentiometer Pin
  pinMode(horPotPin, INPUT);

  //Vertical Motor
  //DDRL |= (1 << vertMotIn1);  // Set pin 46 as output
  //DDRL |= (1 << vertMotIn2);  // Set pin 45 as output
  pinMode(vertMotIn1, OUTPUT);
  pinMode(vertMotIn2, OUTPUT);
  pinMode(vertMotEn, OUTPUT);

  //Vertical Encoder
  pinMode(vertEnA, INPUT_PULLUP);
  pinMode(vertEnB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(vertEnA), enA, RISING);
  attachInterrupt(digitalPinToInterrupt(vertEnB), enB, RISING);

  pinMode(limSw1, INPUT_PULLUP);
  pinMode(limSw2, INPUT_PULLUP);


  Serial.begin(9600);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (incomingByte == 114) { //r
      stpStep(800, 1);
    } else if (incomingByte == 117) { //u
      vertMotDir = 1;
      jogVertical(vertMotDir);
    } else if (incomingByte == 100) { //d
      vertMotDir = 0;
      jogVertical(vertMotDir);
    }
  }

  Serial.println(vertCnt);
  Serial.println(target);

}

//jogs vertical stage 400 counts in the specified direction
//false: down true: up
void jogVertical(bool dir) {

  if (dir) {
    target = vertCnt + 400;
    while (vertCnt < target) {
      SETBIT(PORTL, 4);
      CLEARBIT(PORTL, 3);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    PORTL &= 11100111;
    digitalWrite(vertMotEn, HIGH);

  } else {
    target = vertCnt - 400;
    while (vertCnt > target) {
      SETBIT(PORTL, 3);
      CLEARBIT(PORTL, 4);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    PORTL &= 11100111;
    digitalWrite(vertMotEn, HIGH);
  }
}

//Find travel limits of the vertical stage
void verticalHome() {
  if (digitalRead(limSw1) == HIGH && digitalRead(limSw2) == HIGH) {

    //Lower vertical Stage until it reaches the first limit switch
    while (digitalRead(limSw1) == HIGH) {
      digitalWrite(vertMotIn1, HIGH);
      digitalWrite(vertMotIn2, LOW);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);
    //Reset encoder count to zero
    vertCnt = 0;
    vertTravCnt = vertCnt;

    //Raise vertical stage from  switch
    while (vertCnt < 200) {
      digitalWrite(vertMotIn1, LOW);
      digitalWrite(vertMotIn2, HIGH);
      analogWrite(vertMotEn, 128);
    }

    //Stop motor
    analogWrite(vertMotEn, 0);

    //Approach lower limit switch again slower
    while (digitalRead(limSw1) == HIGH) {
      digitalWrite(vertMotIn1, HIGH);
      digitalWrite(vertMotIn2, LOW);
      analogWrite(vertMotEn, 100);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);

    vertCnt = 0;
    vertTravCnt = vertCnt;

    //Raise vertical stage toward upper limit switch
    while (digitalRead(limSw2) == HIGH) {
      digitalWrite(vertMotIn1, LOW);
      digitalWrite(vertMotIn2, HIGH);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);

    int current = vertCnt;

    //Lower vertical stage from  switch
    while (current - vertCnt < 200) {
      digitalWrite(vertMotIn1, HIGH);
      digitalWrite(vertMotIn2, LOW);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);

    //Approach upper limit switch again slower
    while (digitalRead(limSw2) == HIGH) {
      digitalWrite(vertMotIn1, LOW);
      digitalWrite(vertMotIn2, HIGH);
      analogWrite(vertMotEn, 100);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);

    //Set counts for full vertical travel
    vertTravCnt = vertCnt;

    while (vertCnt > (vertTravCnt * 0.75)) {
      digitalWrite(vertMotIn1, HIGH);
      digitalWrite(vertMotIn2, LOW);
      analogWrite(vertMotEn, 128);
    }
    //Stop motor
    analogWrite(vertMotEn, 0);

  }
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
      delay(1);
      digitalWrite(stpStepPin, LOW);
      delay(1);
    }
  } else {
    Serial.println("Invalid number of steps");
  }
}

void enA() {
  if (digitalRead(vertEnB) == LOW) {
    ++(*cpnt);
  } else {
    --(*cpnt);
  }
}

void enB() {
  if (digitalRead(vertEnA) == LOW) {
    --(*cpnt);
  } else {
    ++(*cpnt);
  }
}



