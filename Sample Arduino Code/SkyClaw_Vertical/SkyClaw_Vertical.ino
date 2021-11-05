/*
   SkyClaw Vertical Travel
   Author: Ben Bradley
*/

#define in1 8
#define inv 9
#define upLimSwitch 13

#define MOTOR_1_PWM in1 // Motor 1 PWM Speed
#define MOTOR_1_DIR inv // Motor 1 Direction

#define vertEnA 2
#define vertEnB 3

//PWM values for motor speed
float Vcal = 0;
int Vmin = 70;
int Vmax = 255;

//Homing command speed
int calibSpeed = 80;

//Encoder resolution
int resolution = 400;

//User specified vertical positon, 0 at top, 40 at bottom
int inPos = 0;
int desPos = 0;
int oldPos = -1;
int newPos = 0;
int posCh = 0;

volatile long enCount = 0;

//Acceleration constant
int ACC = 15000;

//Screw pitch, revolutions/vertical travel
float ratio = 0.25; //in/rev

long x;
long xOld = 0;
// the actual values for "fast" and "slow" depend on the motor
//float Speed=150; // arbitrary fast speed PWM duty cycle
int PWM;

//Counts Deadband
int dx = 20;


void setup() {
  Serial.begin(115200);

  pinMode( MOTOR_1_DIR, OUTPUT );
  pinMode( MOTOR_1_PWM, OUTPUT );
  digitalWrite( MOTOR_1_DIR, LOW );
  digitalWrite( MOTOR_1_PWM, LOW );

  pinMode(upLimSwitch, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(vertEnA), enA, RISING);
  attachInterrupt(digitalPinToInterrupt(vertEnB), enB, RISING);

}

void loop() {

  //Send gantry up to top limit switch
  
  if (digitalRead(upLimSwitch) == HIGH) {
    while (digitalRead(upLimSwitch) == HIGH) {
      digitalWrite( MOTOR_1_DIR, HIGH ); // direction = reverse
      analogWrite( MOTOR_1_PWM, calibSpeed ); // PWM speed
      //Serial.println("In loop");
    }

    digitalWrite( MOTOR_1_DIR, HIGH);
    digitalWrite( MOTOR_1_PWM, LOW );
    
    //Set 0 position for encoder
    enCount = 0;
  }

  inPos = 0;
  while (Serial.available() > 0) {
    byte incoming = Serial.read();
    if (incoming >= '0' && incoming <= '9') {
      inPos = (inPos * 10) + (incoming - '0');
      delay(5);
    }
    else {
      Serial.println("\nInvalid Entry! Try Again.\n");
    }
    
    //Calculate travel distance in encoder pulses
    x = ((resolution * inPos) / (ratio));
  }

  boolean run;
  //set current position to current encoder count
  newPos = enCount;
  // Check if motor reaches to the given position-----
  if (newPos != oldPos)
  {
    oldPos = newPos;
    posCh = x - newPos;

    //1st half of the way
    if (abs(((posCh)) * 100) >= 50 * abs(x - xOld))
    {
      Vcal = Vmin + ((Vmax - Vmin) * abs(newPos - xOld)) / ACC;
      PWM = min(Vmax, Vcal);
      // digitalWrite( GLED, HIGH ); digitalWrite( YLED, LOW );digitalWrite( RLED, LOW );
    }
    //2nd half of the way
    else if (abs((posCh) * 100) < 50 * abs(x - xOld))
    {
      Vcal = Vmin + abs(((Vmax - Vmin) * (posCh)) / ACC);
      PWM = min (Vmax, Vcal);
      //digitalWrite( YLED, HIGH );digitalWrite( GLED, LOW );digitalWrite( RLED, LOW );
    }
    //Serial.print(positionCh);
    //Serial.print ("/");
    float height = (newPos * ratio) / 800;
    Serial.print("Drop Down Height= ");
    Serial.println(height);
  }

  do {
    if (newPos < (x - dx)) {
      digitalWrite( MOTOR_1_DIR, LOW ); // direction = forward
      analogWrite( MOTOR_1_PWM, PWM ); // PWM speed
      run = true;
    }
    if (newPos > (x + dx)) {
      digitalWrite( MOTOR_1_DIR, HIGH ); // direction = reverse
      analogWrite( MOTOR_1_PWM, PWM ); // PWM speed
      run = true;
    }
    if (newPos <= (x + dx) && newPos >= (x - dx)) {
      digitalWrite( MOTOR_1_DIR, HIGH);
      digitalWrite( MOTOR_1_PWM, LOW );
      run = false;
      xOld = x;
    }
  } while (abs(x - newPos) <= dx && run == true);

}

void enA() {
  if (digitalRead(vertEnB) == LOW) {
    ++(enCount);
  } else {
    --(enCount);
  }
}

void enB() {
  if (digitalRead(vertEnA) == LOW) {
    --(enCount);
  } else {
    ++(enCount);
  }
}
