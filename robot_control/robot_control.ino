/*
  few improvement:
    1. change turn45 degree to work using Timer class, as oppose to delay function
    2. front left and front right sensor not detecting at the same time
*/



#include <SharpIR.h>
#include <string.h>

// ---------------------------------------------------------
//                      OUTPUT PINS
// ---------------------------------------------------------

// ---- Movement ----
#include "Movement_Module.h"

// ---------------------------------------------------------
//                      INPUT PINS
// ---------------------------------------------------------

// ---- Sensors ----
#include "Sensor_Module.h"

// ---------------------------------------------------------
//                     Program Logic
// ---------------------------------------------------------

#define MaxHeight 10
#define MinHeight 3

#define MinDistance 20

// Keep track of how many times the robot is moving in a straight line
// if (number > 4) : turn right to take it out of the loop of going back and forth (explore the field)
unsigned short int straightLineTracker = 0;

long int TurnBackTime = 3000.0;

// ---------------------------------------------------------
//                      Helper Functions
// ---------------------------------------------------------


enum ProtocolTypes {
  EdgeAvoidance,
  ObjectAvoidance,
  LineTracking
};

class ProtocolInterface {
public:
  ProtocolInterface(ProtocolTypes Protocol) {
    m_numOfOutputs = 0;
    madeAnyDecisions = false;
  }
  int Log() {
    m_numOfOutputs++;
  }
private:
  int m_numOfOutputs;

  bool madeAnyDecisions;
};

class Timer {
public:
  Timer() {
    m_valueSet = false;
  }

  void m_Start() {
    if (!m_valueSet) {
      m_startTime = millis();
      m_valueSet = true;
    }
  }

  void m_ResetSetting() {
    // m_startTime = ;
    m_valueSet = false;
  }

  bool m_TimerDone(unsigned long timeLimit) {
    unsigned long currentTime = millis();

    if (((currentTime - m_startTime) > timeLimit) && (m_valueSet)) {
      return true;
    }
    return false;
  }

  void m_Print(bool endLine = true) {
    unsigned long currentTime = millis();
    if (endLine) Serial.println(currentTime - m_startTime);
    else Serial.print(currentTime - m_startTime);
  }

  bool m_AlreadyStarted(){
    return m_valueSet;
  }
private:
  bool m_valueSet;
  unsigned long m_startTime;
};

inline bool CheckSensorVal(float DistanceVal) {
  if ((DistanceVal > MaxHeight) || (DistanceVal <= MinHeight)) {
    return true;
  }
  return false;
}

Timer backwardTimer;

inline bool EdgeAvoidanceLogic() {
  bool madeAnyDecisions = false;
  
  // if all wheels are off the floor then stop
  // if (CheckSensorVal(Edge_U_FL) && CheckSensorVal(Edge_U_FR) && CheckSensorVal(Edge_U_BL) && CheckSensorVal(Edge_U_BR)) {
  //   Serial.println("------------ ALL SENSORS HIT ------------ ");
  //   MovementFun::Stop();
  // }
  // else

  if (direction != MovementDir::backward) {
    backwardTimer.m_ResetSetting();
   // SensorFun::Read_EdgeAvoidance_FrontSensors(); // new
    if (CheckSensorVal(Edge_U_FL) && CheckSensorVal(Edge_U_FR)) {
      Serial.println("------------ BOTH FORWARD SENSORS HIT ------------ ");
      MovementFun::Backward();
      straightLineTracker++;
      madeAnyDecisions = true;
    } else if (CheckSensorVal(Edge_U_FL)) {
      Serial.println("------------ FORWARD LEFT SENSOR HIT ------------ ");
      MovementFun::TurnRight45();
      straightLineTracker = 0;
      MovementFun::Forward();
      madeAnyDecisions = true;

    } else if (CheckSensorVal(Edge_U_FR)) {
      Serial.println("------------ FORWARD RIGHT SENSOR HIT ------------ ");
      MovementFun::TurnLeft45();
      straightLineTracker = 0;
      MovementFun::Forward();
      madeAnyDecisions = true;
    }
  } else if ((direction != MovementDir::forward)) {
   // SensorFun::Read_EdgeAvoidance_BackSensors(); // new

    // used to prevent the robot from going backward, when backware sensor is weaker
    // backwardTimer.m_Start();
    // if (!backwardTimer.m_TimerDone(TurnBackTime)) {
    //   // MovementFun::TurnLeft();
    //   MovementFun::TurnLeft45();
    //   MovementFun::TurnLeft45();
    //   MovementFun::TurnLeft45();
    //   MovementFun::Forward();
    //   backwardTimer.m_ResetSetting();
    // }

    if (CheckSensorVal(Edge_U_BL) && CheckSensorVal(Edge_U_BR)) {
      Serial.println("------------ BOTH BACKWARD SENSORS HIT ------------ ");
      MovementFun::Forward();
      straightLineTracker++;
      madeAnyDecisions = true;

      // if the machine has been moving back and forth in a straight line for 2 time (4 sensor hits)
      // Turn to right to make the robot explore the ground
      if (straightLineTracker >= 4) {
        Serial.println("------------ STOPPING ROBOT FROM MOVING IN STRAIGHT LINE (TURNING RIGHT) ------------ ");
        MovementFun::TurnLeft45();
        straightLineTracker = 0;
      }
    } else if (CheckSensorVal(Edge_U_BL)) {
      Serial.println("------------ BACKWARD LEFT SENSOR HIT ------------ ");
      MovementFun::TurnRight45();
      straightLineTracker = 0;
      MovementFun::Forward();
      madeAnyDecisions = true;

    } else if (CheckSensorVal(Edge_U_BR)) {
      Serial.println("------------ BACKWARD RIGHT SENSOR HIT ------------ ");
      MovementFun::TurnLeft45();
      straightLineTracker = 0;
      MovementFun::Forward();
      madeAnyDecisions = true;
    }
  }
  return madeAnyDecisions;
}
// ------- new -------//
// inline bool FlameTrackingLogic(){ // new
//   bool madeAnyDecisions = false;
//   SensorFun::Read_Flame_Sensors();
//   if (F_Flame_IR >= 21){
//     Serial.print("Flame Detected");
//     MovementFun::Forward();
//     madeAnyDecisions = true;
//     if (F_Flame_IR <= 7){
//      MovementFun::Stop();
//    }
//     if (F_Flame_IR == 0){
//       Serial.print("Flame NOT Detected");
//       MovementFun::Forward();
//     }
//   }
//   return madeAnyDecisions;
// }

bool closeEnough = false;
inline bool FlameTrackingLogic(){ // new
  bool madeAnyDecisions = false;
  SensorFun::Read_Flame_Sensors();
  if (!closeEnough){
    if (F_Flame_IR == 0){
      Serial.print("Flame Detected");
      MovementFun::Forward();
      madeAnyDecisions = true;
      SensorFun::Read_ObjectAvoidance_Sensors();
      if(Obj_Sharp_Front < 7){
        MovementFun::Stop();
      } else {
        MovementFun::Forward();
      }
            // Check for R_Flame_IR and turn right45
      if (R_Flame_IR == 0) {
        Serial.print("Flame Detected on Right");
        MovementFun::TurnRight45();
      }

      // Check for L_Flame_IR and turn left45
      if (L_Flame_IR == 0) {
        Serial.print("Flame Detected on Left");
        MovementFun::TurnLeft45();
      }
    //   if (F_Flame_IR == 1){
    //    closeEnough = true;
    //    MovementFun::Stop();
    //  }
    } return madeAnyDecisions;
  }
  // if (closeEnough){
  //   // I need to used the timer class here to run the fan, but for now its enough.
  //   Serial.print("Turning off flame");
  //   MovementFun::TurnRight45();
  //   MovementFun::TurnLeft45();
  //   closeEnough = false; 
  //   madeAnyDecisions = true;
  // }
  // return madeAnyDecisions;
}


inline bool ObjectAvoidanceLogic() {
  bool madeAnyDecisions = false;
  //SensorFun::Read_ObjectAvoidance_Sensors(); // new

  if (Obj_Sharp_Front < 18) {
    Serial.print("Front Sharp: ");
  
    madeAnyDecisions = true;
    if ((Obj_IR_FL == 0) && (Obj_IR_FR == 0)) {
      Serial.print("both");
      MovementFun::Backward();
    }
    if (Obj_IR_FL == 1) {
      Serial.print("Turn Left");
      MovementFun::TurnLeft45();
      MovementFun::Forward();
    } 
    else if (Obj_IR_FR == 1) {
      Serial.print("Turn Right");
      MovementFun::TurnRight45();
      MovementFun::Forward();
    }
  }
  else if (Obj_IR_FL == 0) {
    Serial.print("Corner Sensor: Turn Right");
    madeAnyDecisions = true;
    MovementFun::TurnRight45();
    MovementFun::Forward();
  }
  else if (Obj_IR_FR == 0) {
    Serial.print("Corner Sensor: Turn Left");
    madeAnyDecisions = true;
    MovementFun::TurnLeft45();
    MovementFun::Forward();
  }
  if (direction == MovementDir::backward)
  {
    if (Obj_U_Back < 14)
    {
      MovementFun::Forward();
      madeAnyDecisions = true;
    }
  }
  return madeAnyDecisions;
}

unsigned long timerLimit = 2000;
Timer keepSearchingTimer;
MovementDir sweepDirection;

int numOfSweep = 0;
bool isLineClose = false;

inline bool LineTrackingLogic() {
  bool madeAnyDecisions = false;
 // SensorFun::Read_LineTracking_Sensors(); // new

  // Line following logic
  if (Line_IR_Left || Line_IR_Middle || Line_IR_Right) {
    Serial.println("found Line");
    madeAnyDecisions = true;
    isLineClose = true;
    keepSearchingTimer.m_ResetSetting();
    if (Line_IR_Middle == HIGH) {
      // On the line
      // sweepDirection = MovementDir::forward;
      sweepDirection = MovementDir::forward;
      MovementFun::Forward();
      numOfSweep = 0;
    }

    else if (Line_IR_Right == HIGH) {
      // Turn right
      sweepDirection = MovementDir::turnR;
      MovementFun::TurnRight(10);
      numOfSweep = 0;
    } else if (Line_IR_Left == HIGH) {
      // Turn left
      sweepDirection = MovementDir::turnL;
      MovementFun::TurnLeft();
      numOfSweep = 0;
    }
  } else {
    if (isLineClose) {
      Serial.print("SEARCHING:");
      keepSearchingTimer.m_Start();
      if (!keepSearchingTimer.m_TimerDone(timerLimit)) {
        keepSearchingTimer.m_Print();
        madeAnyDecisions = true;
        if (sweepDirection == MovementDir::turnL) {
          MovementFun::TurnLeft();
        } else {
          MovementFun::TurnRight(10);
        }
      } else {
        Serial.println("Done");
        isLineClose = false;
      }
    } else {
      Serial.println("nothing");
      keepSearchingTimer.m_ResetSetting();
    }

    return madeAnyDecisions;
    // Lost the line - you can add additional logic or routines here
    // if (numOfSweep < 2){
    //   if(timer > timerLimit){
    //     if (numOfSweep < 1){

    //     }
    //     MovementFun::TurnRight();
    //     numOfSweep++;
    //     timer = 0.0;
    //   }
    //   timer += 1.0;
    // }
  }
}

// ---------------------------------------------------------
//                      Base Functions
// ---------------------------------------------------------

void setup() {
  Serial.begin(9600);

  // Initialize motor pins as outputs
  MovementFun::Init();

  // -----------------------------------

  SensorFun::Init();

  Serial.println("-------------");
  Serial.println("Starting Program");
  Serial.println("-------------");


  MovementFun::Forward();
  EdgeAvoidanceLogic();
}

Timer edgeDetectionDelay;
unsigned long edgeDetectionDelayTimeLimit = 1500.0;
bool edgeDetectionDelayDone = true;
void loop() {

  // ReadAllSonicSensors();
  // SensorFun::Read_EdgeAvoidance_BackSensors();
  // SensorFun::Read_EdgeAvoidance_FrontSensors();
  // SensorFun::Read_ObjectAvoidance_Sensors();

  // SensorFun::Print(false, true, false);
  

  // ProtocolInterface test1(ProtocolTypes::ObjectAvoidance);
  // String test = "Hello World";
  // Serial.print("testing String: ");
  // Serial.println(test.c_str());

  // Change to false to only print sensor output and disable output
  int counter = 0;
  if (true) {
    
    // MovementFun::TurnLeft45();
    // if (FlameTrackingLogic()){
    //   Serial.print(F_Flame_IR);
    // }
    bool FlameDetected = FlameTrackingLogic();
     Serial.print(F_Flame_IR);
     Serial.print("\n");
     Serial.print(R_Flame_IR);
     Serial.print("\n");
     Serial.print(L_Flame_IR);
     delay(1000);

    // --------- new comments below ------------
    
    // MovementDir dircheck;
    // bool EdgeAvoidanceHappened = EdgeAvoidanceLogic(); 
    // dircheck = direction;
    // bool ObjectAvoidanceHappened = ObjectAvoidanceLogic(); 
    // Serial.print(Obj_U_Back);
    // if((dircheck == MovementDir::turnL && direction == MovementDir::turnR) || (dircheck == MovementDir::turnR && direction == MovementDir::turnL))
    // {
    //   counter++;
    // }
    // if (counter >= 3) {
    //  MovementFun::Backward();
    //  counter = 0;
    // }
    // if ( (!EdgeAvoidanceHappened) && (!ObjectAvoidanceHappened) ) {
    //   if (edgeDetectionDelayDone) { // lower delay
    //     if (!LineTrackingLogic()) {
    //       if ((direction == MovementDir::turnR) || (direction == MovementDir::turnL)) {
    //         MovementFun::Forward();
    //       }
    //     }
    //   }
    //   else {
    //     edgeDetectionDelay.m_Start();
    //     if (edgeDetectionDelay.m_TimerDone(edgeDetectionDelayTimeLimit)) {
    //       edgeDetectionDelay.m_ResetSetting();
    //       edgeDetectionDelayDone = true;
    //     }
    //   }
    // }
    // else {
    //   edgeDetectionDelayDone = false;
    //   edgeDetectionDelay.m_ResetSetting();
    // }
    // CheckFrontSensor();
    
    
  }
  else {
    
    // MovementFun::Stop();
  }
}