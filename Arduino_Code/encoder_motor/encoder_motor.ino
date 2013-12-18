// Arduino code for motor control.
// Packages.
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Definitions.

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor_2 = AFMS.getMotor(2);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);
char inputtarget_1[2];//the number you enter
char inputtarget_2[2];
char index;
int target_1 = 0; //target position
int target_2 = 0;
int serialavailable = 0;
float error_1 = 0;
float error_2 = 0;

//encoder variables
int encoderPin1 = 2;
int encoderPin2 = 3;

int MSB_1 = 0;
int LSB_1 = 0;

volatile int lastEncoded_1 = 0;
volatile long encoderValue_1 = 0;
long lastencoderValue_1 = 0;
int lastMSB_1 = 0;
int lastLSB_1 = 0;

int encoderPin3 = 4;
int encoderPin4 = 5;
volatile int lastEncoded_2 = 0;
volatile long encoderValue_2 = 0;
long lastencoderValue_2 = 0;
int lastMSB_2 = 0;
int lastLSB_2 = 0;

//light sensor variables
int light_analog_input = A0;
int light_sensor_value = 0;
//temperature sensor variable
int temp_analog_input = A1;
int temp_sensor_value = 0;
long temperature; //Temperature to print
long temperatureC; //Temperature in degrees C

void setup() {
  Serial.begin (115200);

  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  pinMode(encoderPin3, INPUT); 
  pinMode(encoderPin4, INPUT);
  pinMode(light_analog_input, INPUT);
  pinMode(temp_analog_input, INPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder_1, CHANGE); 
  attachInterrupt(1, updateEncoder_1, CHANGE);
  //attachInterrupt(2, updateEncoder_2, CHANGE); 
  //attachInterrupt(3, updateEncoder_2, CHANGE);
  AFMS.begin();
}

void loop(){
  //lightsensor();
  //tempsensor();
  serialavailable = Serial.available();
  if (serialavailable!=0){
    index = Serial.read();
  }
  if (index == '1') {
    encoder_motor_1();
  }
  else {
    encoder_motor_2();
  }
  Serial.print("encoderValue_1/4 = ");
  Serial.println(encoderValue_1);
  Serial.print("MSB_1 = ");
  Serial.println(MSB_1);
  Serial.print("LSB_1 = ");
  Serial.println(LSB_1);
  //delay(1000); //just here to slow down the output, and show it will work  even during a delay
  //Serial.print("target_1 = ");
  //Serial.println(target_1);
  //Serial.print("encoderValue_2/4 = ");
  //Serial.println(encoderValue_2/4);
  delay(1000); //just here to slow down the output, and show it will work  even during a delay
  //Serial.print("target_2 = ");
  //Serial.println(target_2);
}
  


void updateEncoder_1(){
  MSB_1 = digitalRead(encoderPin1); //MSB = most significant bit
  LSB_1 = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded_1 = (MSB_1 << 1) |LSB_1; //converting the 2 pin value to single number
  int sum_1  = (lastEncoded_1 << 2) | encoded_1; //adding it to the previous encoded value

  if(sum_1 == 0b1101 || sum_1 == 0b0100 || sum_1 == 0b0010 || sum_1 == 0b1011) encoderValue_1 ++;
  if(sum_1 == 0b1110 || sum_1 == 0b0111 || sum_1 == 0b0001 || sum_1 == 0b1000) encoderValue_1 --;

  lastEncoded_1 = encoded_1; //store this value for next time
}
void updateEncoder_2(){
  int MSB_2 = digitalRead(encoderPin3); //MSB = most significant bit
  int LSB_2 = digitalRead(encoderPin4); //LSB = least significant bit

  int encoded_2 = (MSB_2 << 1) |LSB_2; //converting the 2 pin value to single number
  int sum_2  = (lastEncoded_2 << 2) | encoded_2; //adding it to the previous encoded value

  if(sum_2 == 0b1101 || sum_2 == 0b0100 || sum_2 == 0b0010 || sum_2 == 0b1011) encoderValue_2 ++;
  if(sum_2 == 0b1110 || sum_2 == 0b0111 || sum_2 == 0b0001 || sum_2 == 0b1000) encoderValue_2 --;

  lastEncoded_2 = encoded_2; //store this value for next time
}
void lightsensor(){
  light_sensor_value = analogRead(light_analog_input);
  Serial.print("Light sensor value = ");
  Serial.println(light_sensor_value);
  delay(100);
}

void tempsensor(){
  long temp_sensor_value = analogRead(temp_analog_input);
  temperatureC = ((temp_sensor_value*5000)/1024) - 500 ; //conversion equation
  temperature = temperatureC/10; //print it in Celsius
  Serial.print("The temperature in celcius is ");
  Serial.println(temperature);
  temperature = (temperature*9)/5 + 32; //Convert from C to F
  Serial.print("The temperature in Fahrenheit is ");
  Serial.println(temperature);
}

void encoder_motor_1(){
  serialavailable = Serial.available();
  if (serialavailable!=0){
    Serial.readBytes(inputtarget_1,2);
    target_1 = atoi(inputtarget_1);
    Serial.print("set to ");
    Serial.println(target_1);
    serialavailable = Serial.available();
    if(serialavailable!=0){
      Serial.readBytes(inputtarget_1,serialavailable);
    }
    serialavailable = 0;
  }
  error_1 = encoderValue_1/4 - target_1;
  Serial.print("error_1 = ");
  Serial.println(error_1);
  if (error_1 == 0) {
    Serial.println("speed set to zero!!!");
    myMotor_1->setSpeed(0);
    myMotor_1->run(FORWARD);
  }
  if (error_1 > 0) {
    myMotor_1->setSpeed(128);
    myMotor_1->run(FORWARD);
  }
  if (error_1 <0) {
    myMotor_1->setSpeed(128);
    myMotor_1->run(BACKWARD);
  }
}

void encoder_motor_2(){
  serialavailable = Serial.available();
  if (serialavailable!=0){
    Serial.readBytes(inputtarget_2,2);
    target_2 = atoi(inputtarget_2);
    Serial.print("set to ");
    Serial.println(target_2);
    serialavailable = Serial.available();
    if(serialavailable!=0){
      Serial.readBytes(inputtarget_2,serialavailable);
    }
    serialavailable = 0;
  }
  error_2 = encoderValue_2/4 - target_2;
  Serial.print("error_2 = ");
  Serial.println(error_2);
  if (error_2 == 0) {
    Serial.println("speed set to zero!!!");
    myMotor_2->setSpeed(0);
    myMotor_2->run(FORWARD);
  }
  if (error_2 > 0) {
    myMotor_2->setSpeed(128);
    myMotor_2->run(FORWARD);
  }
  if (error_2 <0) {
    myMotor_2->setSpeed(128);
    myMotor_2->run(BACKWARD);
  }
}
