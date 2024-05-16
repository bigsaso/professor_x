// ---------------------------------------------------------
//                      Output PINS
// ---------------------------------------------------------

#define MovementEnable 3

#define SpeedRight 5
#define SpeedLeft 6

#define DirRight 7
#define DirLeft 8

// ---------------------------------------------------------
//                      Helper Structs
// ---------------------------------------------------------


enum MovementDir {
  forward,
  backward,
  turnR,
  turnL,
  stop
};
MovementDir direction;

// ---------------------------------------------------------
//                      Helper Functions
// ---------------------------------------------------------


// the functions used to control the movement of the robot
namespace MovementFun {
  // setting the proper pin
  inline void Init(){
    // Initialize motor pins as outputs
    pinMode(SpeedRight, OUTPUT);
    pinMode(SpeedLeft, OUTPUT);

    pinMode(DirRight, OUTPUT);
    pinMode(DirLeft, OUTPUT);

    pinMode(MovementEnable, OUTPUT);
  }


  // Keep the robot forward [stops only when interrupted]
  inline void Forward(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::forward;

    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, HIGH);  // Direction for right wheels
    analogWrite(SpeedRight, 49 + AdjustMotorSpeed);   //speed for  right wheels

    digitalWrite(DirLeft, HIGH);  // Direction for left wheels
    analogWrite(SpeedLeft, 53 + AdjustMotorSpeed);   //speed for left wheels
  }

  // Keep the robot backward [stops only when interrupted]
  inline void Backward(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::backward;

    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, LOW);  // Direction for right wheels
    analogWrite(SpeedRight, 49 + AdjustMotorSpeed);  //speed for  right wheels

    digitalWrite(DirLeft, LOW);  // Direction for left wheels
    analogWrite(SpeedLeft, 53 + AdjustMotorSpeed);  //speed for left wheels
  }

  // turn 45 degree by default
  inline void TurnRight45(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::turnR;
    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, LOW);  // Direction for right wheels
    analogWrite(SpeedRight, 70 + AdjustMotorSpeed);  //speed for  right wheels

    digitalWrite(DirLeft, HIGH);  // Direction for left wheels
    analogWrite(SpeedLeft, 70 + AdjustMotorSpeed);   //speed for left wheels

    delay(375);
  }

  // turn 45 degree by default
  inline void TurnLeft45(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::turnL;
    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, HIGH);  // Direction for right wheels
    analogWrite(SpeedRight, 70 + AdjustMotorSpeed);   //speed for  right wheels

    digitalWrite(DirLeft, LOW);  // Direction for left wheels
    analogWrite(SpeedLeft, 70 + AdjustMotorSpeed);  //speed for left wheels

    delay(375);
  }

  // turn right until stopped
  inline void TurnRight(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::turnR;
    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, LOW);  // Direction for right wheels
    analogWrite(SpeedRight, 50 + AdjustMotorSpeed);  //speed for  right wheels

    digitalWrite(DirLeft, HIGH);  // Direction for left wheels
    analogWrite(SpeedLeft, 50 + AdjustMotorSpeed);   //speed for left wheels
  }

  // turn until stopped
  inline void TurnLeft(short int AdjustMotorSpeed = 0) {
    direction = MovementDir::turnL;
    digitalWrite(MovementEnable, HIGH);

    digitalWrite(DirRight, HIGH);  // Direction for right wheels
    analogWrite(SpeedRight, 50 + AdjustMotorSpeed);   //speed for  right wheels

    digitalWrite(DirLeft, LOW);  // Direction for left wheels
    analogWrite(SpeedLeft, 50 + AdjustMotorSpeed);  //speed for left wheels
  }

  // stop all movement of the robot
  inline void Stop() {
    direction = MovementDir::stop;
    digitalWrite(MovementEnable, LOW);
  }
}
