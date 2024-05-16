#include "Arduino.h"
// ---------------------------------------------------------
//                      INPUT PINS
// ---------------------------------------------------------

// ---- Edge Avoidance ----

#define BLTriggerPIN 52
#define BLEchoPIN 50

#define BRTriggerPIN 46
#define BREchoPIN 48

#define FLTriggerPIN 42
#define FLEchoPIN 44

#define FRTriggerPIN 40
#define FREchoPIN 38

// ---- Object Avoidance ----

// Front Sharp IR
#define Sharp_IRSensor A5


//Front IR sensors
#define FR_IRSensor 36
#define FL_IRSensor 34

//Rear UltraSonic Sensor
#define BackTriggerPIN 13
#define BackEchoPIN 12

// ---- Line Tracking ----
// Front Bottom IR sensors
#define MiddleLinePin A1
#define LeftLinePin A2
#define RightLinePin A0 


SharpIR Sharp_Front(Sharp_IRSensor,1080);//new

// -----------------------------

// Edge Avoidance 
float Edge_U_FL, Edge_U_FR, Edge_U_BL, Edge_U_BR;

// Object Avoidance 
float Obj_Sharp_Front;
uint8_t Obj_IR_FL, Obj_IR_FR;
float Obj_U_Back;

// Line Tracking
uint8_t Line_IR_Left, Line_IR_Middle, Line_IR_Right;


namespace SensorFun{

  inline void Init(){
    // ------------- Edge Avoidance --------------

    pinMode(FRTriggerPIN, OUTPUT);
    pinMode(FREchoPIN,    INPUT);

    pinMode(FLTriggerPIN, OUTPUT);
    pinMode(FLEchoPIN,    INPUT);
    
    pinMode(BLTriggerPIN, OUTPUT);
    pinMode(BLEchoPIN,    INPUT);

    pinMode(BRTriggerPIN, OUTPUT);
    pinMode(BREchoPIN,    INPUT);

    // ------------- Object Avoidance --------------
    // pinMode(Obj_Sharp_Front, INPUT);
    // pinMode(Obj_IR_FL, INPUT);   pinMode(Obj_IR_FR, INPUT);
    // pinMode(Obj_U_Back, INPUT);
    pinMode(BackTriggerPIN, OUTPUT);    
    pinMode(BackEchoPIN,    INPUT);
    // ------------- Line Tracking --------------
    // pinMode(Line_IR_Left, INPUT);
    // pinMode(Line_IR_Middle, INPUT);
    // pinMode(Line_IR_Right, INPUT);
  }

  inline void Print(bool EdgeAvoidance = true, bool ObjectAvoidance = true, bool LineTracking = true) {
    // Print the distance to the serial monitor

    // Serial.println("------------------------------");
    // Serial.println("------------------------------");
    if(EdgeAvoidance){
      Serial.print("Edge Avoidance: ");
      Serial.print("[Ultra Sonic] FL: ");   Serial.print(Edge_U_FL);   Serial.print("cm | ");  Serial.print("FR: ");  Serial.print(Edge_U_FR);   Serial.print("cm | ");
      Serial.print("BL: ");   Serial.print(Edge_U_BL);   Serial.print("cm | ");  Serial.print("BR: ");  Serial.print(Edge_U_BR);   Serial.print("cm");
    }
    Serial.print(" | ");
    if (ObjectAvoidance) {
      Serial.print("OBJECT AVOIDANCE: ");
      Serial.print("[Sharp]Front: "); Serial.print(Obj_Sharp_Front); Serial.print("cm ");
      Serial.print("[IR Sensor]FL: ");   Serial.print(Obj_IR_FL);   Serial.print(" | ");  Serial.print("FR: ");  Serial.print(Obj_IR_FR);   Serial.print(" ");
      Serial.print("[Ultra Sonic]Back: ");   Serial.print(Obj_U_Back);   Serial.print("cm ");
    }
    
    if (LineTracking) {
      Serial.println("Line Tracking: ");
      Serial.print("[IR Sensor]Left: ");   Serial.print(Line_IR_Left);   Serial.print(" | "); Serial.print("Middle: ");   Serial.print(Line_IR_Middle);   Serial.print(" | ");  Serial.print("Right: ");  Serial.print(Line_IR_Right);   Serial.println("");
    }
    Serial.println("");

    // Serial.println("------------------------------");
    // Serial.println("------------------------------");

    // delay(6000);
  }

  // ---------- Sensor Specific ---------- 

  // read the sonic sensor using the give PINs and return the value in CM
  inline long ReadSonicSensors(unsigned short int TriggerPIN, unsigned short int EchoPIN) {
    // Trigger a pulse to measure distance
    digitalWrite(TriggerPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TriggerPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerPIN, LOW);

    // Read the time it takes for the pulse to return
    long duration = pulseIn(EchoPIN, HIGH);

    // Calculate distance in centimeters (or inches) based on the speed of sound
    // Speed of sound in air at room temperature is approximately 343 meters per second (or 0.0343 cm/microsecond)
    float distanceCM = (duration / 2) * 0.0343;

    return distanceCM;
  }

  
  // ---------- Protocol Specific ---------- 
  inline void Read_EdgeAvoidance_FrontSensors() {
    Edge_U_FL = ReadSonicSensors(FLTriggerPIN, FLEchoPIN);
    Edge_U_FR = ReadSonicSensors(FRTriggerPIN, FREchoPIN);
  }
  
  inline void Read_EdgeAvoidance_BackSensors(){
    Edge_U_BL = ReadSonicSensors(BLTriggerPIN, BLEchoPIN);
    Edge_U_BR = ReadSonicSensors(BRTriggerPIN, BREchoPIN);
  }

  inline void Read_ObjectAvoidance_Sensors() {
    Obj_Sharp_Front = Sharp_Front.distance();

    Obj_IR_FR = digitalRead(FR_IRSensor);
    Obj_IR_FL = digitalRead(FL_IRSensor);

    Obj_U_Back = ReadSonicSensors(BackTriggerPIN, BackEchoPIN);
  }

  inline void Read_LineTracking_Sensors() {
    Line_IR_Left = digitalRead(LeftLinePin);
    Line_IR_Middle = digitalRead(MiddleLinePin);
    Line_IR_Right = digitalRead(RightLinePin);
  }


  inline void Read_All_Sensors(){
    Read_EdgeAvoidance_FrontSensors();
    Read_EdgeAvoidance_BackSensors();
    Read_ObjectAvoidance_Sensors();
    Read_LineTracking_Sensors();
  }

  // Functions to print objects and/or edges
  inline void PrintObjectDetection() {
    if (Obj_Sharp_Front < 18 && Obj_IR_FL != 0 && Obj_IR_FR != 0) Serial.println("Object detected in front sharp");
    if (Obj_Sharp_Front >= 18 && Obj_IR_FL == 0 && Obj_IR_FR != 0) Serial.println("Object detected in front IR left");
    if (Obj_Sharp_Front >= 18 && Obj_IR_FL != 0 && Obj_IR_FR == 0) Serial.println("Object detected in front IR right");
    if (Obj_Sharp_Front < 18 && Obj_IR_FL == 0 && Obj_IR_FR == 0) Serial.println("Object detected in front");
    if (Obj_Sharp_Front < 18 && Obj_IR_FL == 0 && Obj_IR_FR != 0) Serial.println("Object detected in front left");
    if (Obj_Sharp_Front < 18 && Obj_IR_FL != 0  && Obj_IR_FR == 0) Serial.println("Object detected in front right");
    if (Obj_U_Back < 14) Serial.println("Object detected in back");
  }

  inline bool CheckSensorVal(float DistanceVal) {
    if ((DistanceVal > 10) || (DistanceVal <= 3)) {
      return true;
    }
    return false;
  }

  inline void PrintEdgeDetection() {
    if (CheckSensorVal(Edge_U_FL) && !CheckSensorVal(Edge_U_FR)) Serial.println("Edge detected in front left");
    if (!CheckSensorVal(Edge_U_FL) && CheckSensorVal(Edge_U_FR)) Serial.println("Edge detected in front right");
    if (CheckSensorVal(Edge_U_BL) && !CheckSensorVal(Edge_U_BR)) Serial.println("Edge detected in back left");
    if (!CheckSensorVal(Edge_U_BL) && CheckSensorVal(Edge_U_BR)) Serial.println("Edge detected in back right");
    if (CheckSensorVal(Edge_U_FL) && CheckSensorVal(Edge_U_FR)) Serial.println("Edge detected in front");
    if (CheckSensorVal(Edge_U_BL) && CheckSensorVal(Edge_U_BR)) Serial.println("Edge detected in back");
  }
}
