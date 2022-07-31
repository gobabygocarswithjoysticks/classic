//**********************Arduino code for making a go-baby-go car with a joystick and two motors!!!! vroom! vroom! :) ***************************
//Go to https://www.arduino.cc/en/Reference/HomePage for more information on the commands used in this program.
//////////////////////////////////////CHANGE THESE NUMBERS TO CALIBRATE AND MODIFY THE CAR'S DRIVING
#define CONTROL_RIGHT 632  //use to calibrate joystick (value from the X axis of the joystick when all the way to the left)
#define CONTROL_CENTER_X 509 //use to calibrate joystick (value from the X axis of the joystick when it is centered)
#define CONTROL_LEFT 385  //use to calibrate joystick (value from the X axis of the joystick when all the way to the right)
#define CONTROL_UP 638  //use to calibrate joystick (value from the Y axis of the joystick when all the way to the bottom)
#define CONTROL_CENTER_Y 511 //use to calibrate joystick (value from the Y axis of the joystick when it is centered)
#define CONTROL_DOWN 380  //use to calibrate joystick (value from the Y axis of the joystick when all the way to the top)
#define ACCELERATION_FORWARD   .17      //change # to change the amount of acceleration when going forward (fast acceleration could be uncomfortable) bigger number=quicker acceleration (slowly change the numbers)
#define DECELERATION_FORWARD   .4    //change # to change the amount of deceleration when going forward (fast acceleration could be uncomfortable) bigger number=quicker acceleration (slowly change the numbers)
#define ACCELERATION_BACKWARD  .1      //change # to change the amount of acceleration when going backward (fast acceleration could be uncomfortable) bigger number=quicker acceleration (slowly change the numbers)
#define DECELERATION_BACKWARD  .5    //change # to change the amount of deceleration when going backward (fast acceleration could be uncomfortable) bigger number=quicker acceleration (slowly change the numbers)
#define FASTEST_FORWARD 1725  //change # to limit the forward speed 2000=max speed  1500=not at all
#define FASTEST_BACKWARD 1275 //change # to limit the backward speed 1000=max speed 1500=not at all
#define TURN_SPEED .5 //change # to limit the turning speed 0=none 1=full power (the truck might turn too quickly)
#define SLOW_TURNING_WHEN_MOVING .5 // 0-1 what amount of TURN_SPEED to use when moving forward or backward (to be able to veer to a side more smoothly when moving forward)
///////////////////////////////////pins on the arduino////////////////////////////////////////////////////////////////////
#define joyXPin 15//------------pin connected to the "X" axis of the joystick
#define joyYPin 16//------------pin connected to the "Y" axis of the joystick
#define leftMotorControllerPin 10//--pin connected to the signal pin of the left motor controller
#define rightMotorControllerPin 9//--pin connected to the signal pin of the right motor controller
/////////////////////////////////motorcontroller values/////////////
#define startGoingLargeVal 1517//when the motorcontroller starts moving
#define startGoingSmallVal 1483//when the motorcontroller starts moving
///////////////////////////////////////////////
#include <Servo.h> //servo library
/////////////////////////////////////variables
Servo rightMotorController;  //for motor controller (uses servo signals)
Servo leftMotorController;  //for motor controller (uses servo signals)
float leftMotorWriteVal = 1500.00; //variable to give to the motor controller
float rightMotorWriteVal = 1500.00; //variable to give to the motor controller
float rightMotorTryVal = 1500.00; //variable to end up giving to the motor controller (this is before acceleration is calculated)
float leftMotorTryVal = 1500.00; //variable to end up giving to the motor controller (this is before acceleration is calculated)
float joyXVal = 0000.000; //variable read from the "X" axis of the joystick
float joyYVal = 0000.000; //variable read from the "Y" axis of the joystick
unsigned int timezie = 0; //(cute) variable to help with the timing of printing values for debugging and calibrating
float speedVal = 1500.000; //forward speed
float turnVal = 0000.000; // how much to turn
//////////////////////////////////////////////////////////
void setup() {  //run once when the car is turned on to set up stuff (pin modes)
  Serial.begin(19200); //start serial port  (for finding problems and calibrating)
  Serial.println();//send message over serial port
  Serial.println("  wait 5 seconds for the ESC to calibrate itself");//send message over serial port
  Serial.println("current values:");
  Serial.println("---------");
  Serial.print("CONTROL_RIGHT: ");
  Serial.println(CONTROL_RIGHT);
  Serial.print("CONTROL_CENTER_X: ");
  Serial.println(CONTROL_CENTER_X);
  Serial.print("CONTROL_LEFT: ");
  Serial.println(CONTROL_LEFT);
  Serial.print("CONTROL_UP: ");
  Serial.println(CONTROL_UP);
  Serial.print("CONTROL_CENTER_Y: ");
  Serial.println(CONTROL_CENTER_Y);
  Serial.print("CONTROL_DOWN: ");
  Serial.println(CONTROL_DOWN);
  Serial.print("ACCELERATION_FORWARD: ");
  Serial.println(ACCELERATION_FORWARD);
  Serial.print("DECELERATION_FORWARD: ");
  Serial.println(DECELERATION_FORWARD);
  Serial.print("ACCELERATION_BACKWARD: ");
  Serial.println(ACCELERATION_BACKWARD);
  Serial.print("DECELERATION_BACKWARD: ");
  Serial.println(DECELERATION_BACKWARD);
  Serial.print("FASTEST_FORWARD: ");
  Serial.println(FASTEST_FORWARD);
  Serial.print("FASTEST_BACKWARD: ");
  Serial.println(FASTEST_BACKWARD);
  Serial.print("TURN_SPEED: ");
  Serial.println(TURN_SPEED);
  Serial.print("SLOW_TURNING_WHEN_MOVING");
  Serial.println(SLOW_TURNING_WHEN_MOVING);
  Serial.print("joyXPin: ");
  Serial.println(joyXPin);
  Serial.print("joyYPin: ");
  Serial.println(joyYPin);
  Serial.print("leftMotorControllerPin: ");
  Serial.println(leftMotorControllerPin);
  Serial.print("rightMotorControllerPin: ");
  Serial.println(rightMotorControllerPin);
  Serial.print("startGoingLargeVal: ");
  Serial.println(startGoingLargeVal);
  Serial.print("startGoingSmallVal: ");
  Serial.println(startGoingSmallVal);
  Serial.println();
  pinMode(joyXPin, INPUT); //set joystick pin as an input
  pinMode(joyYPin, INPUT); //set joystick pin as an input
  leftMotorController.attach(leftMotorControllerPin);//use pin for the motor controller
  rightMotorController.attach(rightMotorControllerPin);//use pin for the motor controller
  leftMotorController.writeMicroseconds(1500);//tell the motor controller to not move
  rightMotorController.writeMicroseconds(1500);//tell the motor controller to not move
  delay(3000);//wait for the motor controller to calibrate itself
  leftMotorController.writeMicroseconds(1540);//pulse motors slightly
  rightMotorController.writeMicroseconds(1540);//pulse motors slightly
  delay(250);//pause while motors are pulsing
  leftMotorController.writeMicroseconds(1500);//stop the motors
  rightMotorController.writeMicroseconds(1500);//stop the motors
}
/////////////////////////////////////////////////////////
void loop() { //runs over and over (main stuff)
  ///////////////////////////////////////////////////////////input code
  joyXVal = analogRead(joyXPin); //read joystick input and save it to a variable
  joyYVal = analogRead(joyYPin); //read joystick input and save it to a variable
  ////////////////////////////////////////////joystick code//////////////////

  if (CONTROL_LEFT < CONTROL_RIGHT) { //if the value for when the joystick is towards the left is less then the value for when the joystick is towards the right...
    if (joyXVal < CONTROL_CENTER_X) { //...and if going left...
      turnVal = map(joyXVal, CONTROL_LEFT, CONTROL_CENTER_X, TURN_SPEED * -500, 0); //set turnVal from the joystick
    }
    if (joyXVal > CONTROL_CENTER_X) {
      turnVal = map(joyXVal, CONTROL_RIGHT, CONTROL_CENTER_X, TURN_SPEED * 500, 0);
    }
  }
  if (CONTROL_LEFT > CONTROL_RIGHT) {
    if (joyXVal > CONTROL_CENTER_X) {
      turnVal = map(joyXVal, CONTROL_LEFT, CONTROL_CENTER_X, TURN_SPEED * -500, 0);
    }
    if (joyXVal < CONTROL_CENTER_X) {
      turnVal = map(joyXVal, CONTROL_RIGHT, CONTROL_CENTER_X, TURN_SPEED * 500, 0);
    }
  }


  if (CONTROL_UP < CONTROL_DOWN) {
    if (joyYVal < CONTROL_CENTER_Y) {
      speedVal = map(joyYVal, CONTROL_UP, CONTROL_CENTER_Y, FASTEST_FORWARD, 1500);
    }
    if (joyYVal > CONTROL_CENTER_Y) {
      speedVal = map(joyYVal, CONTROL_DOWN, CONTROL_CENTER_Y, FASTEST_BACKWARD, 1500);
    }
  }
  if (CONTROL_UP > CONTROL_DOWN) {
    if (joyYVal > CONTROL_CENTER_Y) {
      speedVal = map(joyYVal, CONTROL_UP, CONTROL_CENTER_Y, FASTEST_FORWARD, 1500);
    }
    if (joyYVal < CONTROL_CENTER_Y) {
      speedVal = map(joyYVal, CONTROL_DOWN, CONTROL_CENTER_Y, FASTEST_BACKWARD, 1500);
    }
  }
  ///////slow turning proportionally to speed of car
  if (speedVal >= 1500) {
    turnVal = map(turnVal, TURN_SPEED * -500, TURN_SPEED * 500, map(abs(speedVal - 1500), 0, FASTEST_FORWARD - 1500, TURN_SPEED * -500, TURN_SPEED * -SLOW_TURNING_WHEN_MOVING * 500), map(abs(speedVal - 1500), 0, FASTEST_FORWARD - 1500, TURN_SPEED * 500, TURN_SPEED * SLOW_TURNING_WHEN_MOVING * 500));
  }
  else {
    turnVal = map(turnVal, TURN_SPEED * -500, TURN_SPEED * 500, map(abs(1500 - speedVal), 0, 1500 - FASTEST_BACKWARD, TURN_SPEED * -500, TURN_SPEED * -SLOW_TURNING_WHEN_MOVING * 500), map(abs(1500 - speedVal), 0, 1500 - FASTEST_BACKWARD, TURN_SPEED * 500, TURN_SPEED * SLOW_TURNING_WHEN_MOVING * 500));
  }
  leftMotorTryVal = speedVal + turnVal; //use the forward-backward speed value and the turn value to find the left motor value
  rightMotorTryVal = speedVal - turnVal; //use the forward-backward speed value and the turn value to find the right motor value
  ////////////////////////////////////code to make the car accelerate slowly
  ////for left motor
  if (leftMotorTryVal < startGoingLargeVal && leftMotorTryVal > startGoingSmallVal) { //if the motor wouldn't be moving enough anyway...
    leftMotorTryVal = 1500; //...don't turn on the motor.
  }
  if (leftMotorTryVal < 1500) { //if going backwards...
    if (leftMotorTryVal <= leftMotorWriteVal - ACCELERATION_BACKWARD) { //...if trying to go faster backwards...
      leftMotorWriteVal = leftMotorWriteVal - ACCELERATION_BACKWARD; //...accelerate by ACCELERATION_BACKWARD
    }
    if (leftMotorTryVal > leftMotorWriteVal + DECELERATION_BACKWARD) { //if trying to go slower backwards...
      leftMotorWriteVal = leftMotorWriteVal + DECELERATION_BACKWARD; //...decelerate by DECELERATION_BACKWARD
    }
  }
  if (leftMotorTryVal >= 1500) { //if going forwards...
    if (leftMotorTryVal >= leftMotorWriteVal + ACCELERATION_FORWARD) { //...if trying to go faster forwards...
      leftMotorWriteVal = leftMotorWriteVal + ACCELERATION_FORWARD; //...accelerate by ACCELERATION_FORWARD
    }
    if (leftMotorTryVal < leftMotorWriteVal - DECELERATION_FORWARD) { // if trying to go slower forwards...
      leftMotorWriteVal = leftMotorWriteVal - DECELERATION_FORWARD; //...decelerate by DECELERATION_FORWARD
    }
  }
  ////////for right motor
  if (rightMotorTryVal < startGoingLargeVal && rightMotorTryVal > startGoingSmallVal) { //if the motor wouldn't be moving enough anyway...
    rightMotorTryVal = 1500; //...don't turn on the motor.
  }
  if (rightMotorTryVal < 1500) { //if going backwards...
    if (rightMotorTryVal <= rightMotorWriteVal - ACCELERATION_BACKWARD) { //...if trying to go faster backwards...
      rightMotorWriteVal = rightMotorWriteVal - ACCELERATION_BACKWARD; //...accelerate by ACCELERATION_BACKWARD
    }
    if (rightMotorTryVal > rightMotorWriteVal + DECELERATION_BACKWARD) { //if trying to go slower backwards...
      rightMotorWriteVal = rightMotorWriteVal + DECELERATION_BACKWARD; //...decelerate by DECELERATION_BACKWARD
    }
  }
  if (rightMotorTryVal >= 1500) { //if going forwards...
    if (rightMotorTryVal >= rightMotorWriteVal + ACCELERATION_FORWARD) { //...if trying to go faster forwards...
      rightMotorWriteVal = rightMotorWriteVal + ACCELERATION_FORWARD; //...accelerate by ACCELERATION_FORWARD
    }
    if (rightMotorTryVal < rightMotorWriteVal - DECELERATION_FORWARD) { // if trying to go slower forwards...
      rightMotorWriteVal = rightMotorWriteVal - DECELERATION_FORWARD; //...decelerate by DECELERATION_FORWARD
    }
  }
  ///////////////////////////////////////////////send values to servo and motor controller////////////////////////
  leftMotorWriteVal = constrain(leftMotorWriteVal, FASTEST_BACKWARD, FASTEST_FORWARD); //make sure the value is in the right range
  rightMotorWriteVal = constrain(rightMotorWriteVal, FASTEST_BACKWARD, FASTEST_FORWARD); //make sure the value is in the right range
  rightMotorController.writeMicroseconds(int(rightMotorWriteVal)); //send a servo signal to the motor controller for the Motor value (after acceleration)
  leftMotorController.writeMicroseconds(int(leftMotorWriteVal)); //send a servo signal to the motor controller for the Motor value (after acceleration)
  /////////////////////////////////////print values to help figure out problems
  if (timezie > 500) { //around twice a second
    timezie = 0;
    Serial.print("X joystick value: ");
    Serial.print(joyXVal); Serial.print(","); //joystickX value
    Serial.print("Y joystick value: ");
    Serial.print(joyYVal); Serial.print(","); //joystickY value
    Serial.println();
    Serial.print("speedVal: ");
    Serial.println(speedVal);
    Serial.print("turnVal: ");
    Serial.println(turnVal);
    Serial.println();
    Serial.println("motor values: ");
    Serial.print(leftMotorTryVal); Serial.print(","); //value before slow acceleration is calculated in
    Serial.print(rightMotorTryVal); Serial.print(","); //value sent to the motor controller
    Serial.println();
    Serial.print(leftMotorWriteVal); Serial.print(","); //value before slow acceleration is calculated in
    Serial.print(rightMotorWriteVal); Serial.print(","); //value sent to the motor controller
    Serial.println();
    Serial.println();
    Serial.println();
  }
  timezie++;
}
