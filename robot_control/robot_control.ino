/*
  few improvement:
    1. change turn45 degree to work using Timer class, as oppose to delay function
    2. front left and front right sensor not detecting at the same time
*/

#include <FastLED.h>
#include <SharpIR.h>
#include <string.h>

// ---------------------------------------------------------
//                      OUTPUT PINS
// ---------------------------------------------------------

// ---- Movement ----
#include "Movement_Module.h"

// ---- LEDs ----
#define LED_PIN 4  // Pin connected to the LED
#define NUM_LEDS 1 // Number of LEDs
#define BRIGHTNESS 20 // Initial brightness (adjust as needed)

CRGB leds[NUM_LEDS]; // Define an array to store the LED colors

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

bool lastButtonState = LOW;

// ---------------------------------------------------------
//                      Helper Functions
// ---------------------------------------------------------

inline bool CheckSensorVal(float DistanceVal) {
  if ((DistanceVal > MaxHeight) || (DistanceVal <= MinHeight)) {
    return true;
  }
  return false;
}

inline bool EdgeAvoidanceLogic() {
  // Read edge avoidance sensors
  SensorFun::Read_EdgeAvoidance_FrontSensors();
  SensorFun::Read_EdgeAvoidance_BackSensors();
  // Check if any edge is detected
  if (CheckSensorVal(Edge_U_FL) || CheckSensorVal(Edge_U_FR) || CheckSensorVal(Edge_U_BL) || CheckSensorVal(Edge_U_BR)) {
    // Edge detected, stop movement
    return true;
  }
  // No edge detected, continue movement
  return false;
} 

inline bool ObjectAvoidanceLogic() {
  SensorFun::Read_ObjectAvoidance_Sensors();
  if (Obj_Sharp_Front < 18 || Obj_IR_FL == 0 || Obj_IR_FR == 0 || Obj_U_Back < 14) {
    // Object detected, stop movement
    return true;
  } 
  // No object detected
  return false;
}

// Function to change the color and brightness of the LED
void changeLedColor(CRGB color, uint8_t brightness = BRIGHTNESS) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;                // Set the LED color
  }
  FastLED.setBrightness(brightness); // Set the brightness
  FastLED.show();                   // Update the LED with the new color and brightness
}

// ---------------------------------------------------------
//                      Base Functions
// ---------------------------------------------------------

void setup() {
  Serial.begin(9600);
  // Initialize motor pins as outputs
  MovementFun::Init();
  // Initialize LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); // Initialize FastLED
  FastLED.setBrightness(BRIGHTNESS); // Set the initial brightness
  changeLedColor(CRGB::Black, 0);  // Set to black with brightness 0
  // Initialize sensor pins
  SensorFun::Init();
  Serial.println("-------------");
  Serial.println("Starting Program");
  Serial.println("-------------");
  // Set the button pin as input
  pinMode(32, INPUT_PULLUP);
}

bool robotActive = false;

void loop() {
  // Update robot state
  int buttonState = digitalRead(32);
  if(buttonState != lastButtonState) {
    if(buttonState == HIGH) {
      robotActive = !robotActive;
      if(robotActive) {
        Serial.println("Robot is now active. Awaiting command...");
        changeLedColor(CRGB::Green);
      }
      else {
        Serial.println("Robot is now inactive. Stopping...");
        MovementFun::Stop();
        changeLedColor(CRGB::Red);
      }
    }
    delay(10);
  }
  lastButtonState = buttonState;
  // Read GPIO signals from Raspberry Pi for movement control
  int rightSignal = digitalRead(RightGPIO);
  int leftSignal = digitalRead(LeftGPIO);
  int backwardSignal = digitalRead(BackwardGPIO);
  int forwardSignal = digitalRead(ForwardGPIO);
  // Check sensor readings
  SensorFun::Read_EdgeAvoidance_FrontSensors();
  SensorFun::Read_EdgeAvoidance_BackSensors();
  // React to sensor readings and user signals
  bool objectDetected = ObjectAvoidanceLogic();
  bool edgeDetected = EdgeAvoidanceLogic();
  if (robotActive && objectDetected) {
    // If an obstacle or edge is detected, stop movement in that direction
    MovementFun::Stop();
    //Serial.println("Object Detected ");
    SensorFun::PrintObjectDetection();
    if (Obj_Sharp_Front < 18 && Obj_IR_FL == 0 && Obj_IR_FR == 0) {
      if (backwardSignal == HIGH) {
        // If backward signal is received, move backward
        MovementFun::Backward();
      }
    }
    else if (Obj_Sharp_Front < 18 && Obj_IR_FL == 0) {
      if (rightSignal == HIGH) {
        MovementFun::TurnRight45();
        Serial.println("Front left object detected, turning right");
      }
      else if (backwardSignal == HIGH) {
        MovementFun::Backward();
        Serial.println("Front left object detected, moving backward");
      }      
    }
    else if (Obj_Sharp_Front < 18 && Obj_IR_FR == 0) {
      if (leftSignal == HIGH) {
        MovementFun::TurnLeft45();
        Serial.println("Front right object detected, turning left");
      }
      else if (backwardSignal == HIGH) {
        MovementFun::Backward();
        Serial.println("Front right object detected, moving backward");
      }      
    }
    else if (Obj_Sharp_Front < 18) {
      if (leftSignal == HIGH) {
        MovementFun::TurnLeft45();
        Serial.println("Object detected in the front, turning left");
      }
      else if (backwardSignal == HIGH) {
        MovementFun::Backward();
        Serial.println("Object detected in the frontd, moving backward");
      }  
      else if (rightSignal == HIGH) {
        MovementFun::TurnRight45();
        Serial.println("Object detected in the front, turning right");
      }            
    }
    else if (Obj_IR_FR == 0) {
      if (leftSignal == HIGH) {
        MovementFun::TurnLeft45();
        Serial.println("Front right object detected, turning left");
      }
      else if (backwardSignal == HIGH) {
        MovementFun::Backward();
        Serial.println("Front right object detected, moving backward");
      }              
    }
    else if (Obj_IR_FL == 0) {
      if (rightSignal == HIGH) {
        MovementFun::TurnRight45();
        Serial.println("Front left object detected, turning right");
      }
      else if (backwardSignal == HIGH) {
        MovementFun::Backward();
        Serial.println("Front left object detected, moving backward");
      }              
    }
    else if (Obj_U_Back < 14) {
      if (backwardSignal == HIGH) {
        MovementFun::Stop();
        Serial.println("Object detected in the back, stopping");
      }              
    }             
  }
  else if (robotActive && edgeDetected){
    MovementFun::Stop();
    // Serial.println("Edge Detected ");
    SensorFun::PrintEdgeDetection();
    if (CheckSensorVal(Edge_U_FL) && CheckSensorVal(Edge_U_FR)) {
      if (backwardSignal == HIGH) {
        // If backward signal is received, move backward
        MovementFun::Backward();
      }
      else if (CheckSensorVal(Edge_U_FL)) {
        // Front left edge detected, move backward or turn right if a signal is received
        if (rightSignal == HIGH) {
          MovementFun::TurnRight45();
          Serial.println("Front left edge detected, turning right");
        }
        else if (backwardSignal == HIGH) {
          MovementFun::Backward();
          Serial.println("Front left edge detected, moving backward");
        }
      }
      else if (CheckSensorVal(Edge_U_FR)) {
        // Front right edge detected, move backward or turn left if a signal is received
        if (leftSignal == HIGH) {
          MovementFun::TurnLeft45();
          Serial.println("Front right edge detected, turning left");
        }
        else if (backwardSignal == HIGH) {
          MovementFun::Backward();
          Serial.println("Front right edge detected, moving backward");
        }
      }
    }
    if (CheckSensorVal(Edge_U_BL) && CheckSensorVal(Edge_U_BR)) {
      if (forwardSignal == HIGH) {
        // If backward signal is received, move backward
        MovementFun::Forward();
      }
      else if (CheckSensorVal(Edge_U_BL)) {
        // Front left edge detected, move backward or turn right if a signal is received
        if (rightSignal == HIGH) {
          MovementFun::TurnRight45();
          Serial.println("Back left edge detected, turning right");
        }
        else if (forwardSignal == HIGH) {
          MovementFun::Forward();
          Serial.println("Back left edge detected, moving forward");
        }
      }
      else if (CheckSensorVal(Edge_U_BR)) {
        // Front right edge detected, move backward or turn left if a signal is received
        if (leftSignal == HIGH) {
          MovementFun::TurnLeft45();
          Serial.println("Back right edge detected, turning left");
        }
        else if (forwardSignal == HIGH) {
          MovementFun::Forward();
          Serial.println("Back right edge detected, moving forward");
        }
      }
    }
  }
  else {
    // If no obstacle or edge is detected, react to user signals
    if (rightSignal == HIGH && robotActive) {
      MovementFun::TurnRight45();
      Serial.println("Turning right");
    } 
    if (leftSignal == HIGH && robotActive) {
      MovementFun::TurnLeft45();
      Serial.println("Turning left");
    } 
    if (backwardSignal == HIGH && robotActive) {
      MovementFun::Backward();
      Serial.println("Moving Back");
    } 
    if (forwardSignal == HIGH && robotActive) {
      MovementFun::Forward();
      Serial.println("Moving Forward");
    }
  }
}
