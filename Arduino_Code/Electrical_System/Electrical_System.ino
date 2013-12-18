// Arduino code for motor control.
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
int target_1 = 0; //target position
int target_2 = 0;
float error_1 = 0;
float error_2 = 0;
String inputtarget = "";
char data;


//light sensor variables
int light_analog_input = A0;
int light_sensor_value = 0;
//temperature sensor variable
int temp_analog_input = A1;
int temp_sensor_value = 0;
long temperature; //Temperature to print
long temperatureC; //Temperature in degrees C

//encoder variables
 //encoder 1 (small encoder)
int encoderPin1 = 6;
int encoderPin2 = 7;

volatile int lastEncoded_1 = 0;
volatile long encoderValue_1 = 0;

long lastencoderValue_1 = 0;
int currentMSB_1 = 0;
int currentLSB_1 = 0;
int lastMSB_1 = 0;
int lastLSB_1 = 0;

 //encoder 2 (big encoder)
int encoderPin3 = A2;
int encoderPin4 = A3;

volatile int lastEncoded_2 = 0;
volatile long encoderValue_2 = 0;

long lastencoderValue_2 = 0;
int currentMSB_2 = 0;
int currentLSB_2 = 0;
int lastMSB_2 = 0;
int lastLSB_2 = 0;
unsigned long time = 0;

void setup() {
  Serial.begin (115200);
  pinMode(light_analog_input, INPUT);
  pinMode(temp_analog_input, INPUT);
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(encoderPin3, INPUT);
  pinMode(encoderPin4, INPUT);
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);
  //digitalWrite(encoderPin3, HIGH);
  //digitalWrite(encoderPin4, HIGH);
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  //attachInterrupt(0, updateEncoder_1, CHANGE); 
  //attachInterrupt(1, updateEncoder_1, CHANGE);
  AFMS.begin();
}

void loop() {
  //lightsensor();
  //tempsensor();
  inputtarget = "";
  /*while (Serial.available()){
    data = Serial.read();
    if (data=='a'){
      break;
    }  
    inputtarget = inputtarget + data;
    target_1 = inputtarget.toInt();
  }*/
  currentMSB_1 = digitalRead(encoderPin1);
  currentLSB_1 = digitalRead(encoderPin2);
  currentMSB_2 = analogRead(encoderPin3);
  currentLSB_2 = analogRead(encoderPin4);
   if (currentMSB_2 >= 400){
    currentMSB_2 = HIGH;
  }
  else{
    currentMSB_2 = LOW;
  }
  if (currentLSB_2 >= 400){
    currentLSB_2 = HIGH;
  }
  else{
    currentLSB_2 = LOW;
  }
  if (currentMSB_1 != lastMSB_1 || currentLSB_1 !=lastLSB_1){
    updateEncoder_1();
  }  
  if (currentMSB_2 != lastMSB_2 || currentLSB_2 !=lastLSB_2){
    updateEncoder_2(currentMSB_2, currentLSB_2);
  }
  lastMSB_1 = currentMSB_1;
  lastLSB_1 = currentLSB_1;  
  lastMSB_2 = currentMSB_2;
  lastLSB_2 = currentLSB_2;
  motor_1();
  motor_2();
  if (millis() - time >= 500){
    //Serial.print("target_1 is ");
    //Serial.println(target_1);
    //Serial.print("EncoderValue_1 is ");
    //Serial.println(encoderValue_1);
    Serial.print("target_2 is ");
    Serial.println(target_2);
    Serial.print("EncoderValue_2 is ");
    Serial.println(encoderValue_2);    
    time = millis();
  }
}



void lightsensor(){
  light_sensor_value = analogRead(light_analog_input);
  Serial.print("Light sensor value = ");
  Serial.println(light_sensor_value);
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

void updateEncoder_1(){
  int MSB_1 = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB_1 = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded_1 = (MSB_1 << 1) |LSB_1; //converting the 2 pin value to single number
  int sum_1  = (lastEncoded_1 << 2) | encoded_1; //adding it to the previous encoded value

  if(sum_1 == 0b1101 || sum_1 == 0b0100 || sum_1 == 0b0010 || sum_1 == 0b1011) encoderValue_1 ++;
  if(sum_1 == 0b1110 || sum_1 == 0b0111 || sum_1 == 0b0001 || sum_1 == 0b1000) encoderValue_1 --;

  lastEncoded_1 = encoded_1; //store this value for next time
}

void updateEncoder_2(int currentMSB_2, int currentLSB_2){
  int MSB_2 = currentMSB_2; //MSB = most significant bit
  int LSB_2 = currentLSB_2; //LSB = least significant bit

  int encoded_2 = (MSB_2 << 1) |LSB_2; //converting the 2 pin value to single number
  int sum_2  = (lastEncoded_2 << 2) | encoded_2; //adding it to the previous encoded value

  if(sum_2 == 0b1101 || sum_2 == 0b0100 || sum_2 == 0b0010 || sum_2 == 0b1011) encoderValue_2 ++;
  if(sum_2 == 0b1110 || sum_2 == 0b0111 || sum_2 == 0b0001 || sum_2 == 0b1000) encoderValue_2 --;

  lastEncoded_2 = encoded_2; //store this value for next time
}

void motor_1(){
  error_1 = target_1 - encoderValue_1;
  if (error_1 <= 2 && error_1 >= -2) {
    //Serial.println("speed set to zero!!!");
    myMotor_1->setSpeed(0);
    myMotor_1->run(FORWARD);
  }
  else if (error_1 > 0) {
    myMotor_1->setSpeed(128);
    myMotor_1->run(FORWARD);
  }
  else if (error_1 <0) {
    myMotor_1->setSpeed(128);
    myMotor_1->run(BACKWARD);
  } 
}
void motor_2(){  
  error_2 = target_2 - encoderValue_2;
  if (error_2 <= 2 && error_2 >= -2) {
    //Serial.println("speed set to zero!!!");
    myMotor_2->setSpeed(0);
    myMotor_2->run(FORWARD);
  }
  else if (error_2 > 0) {
    myMotor_2->setSpeed(128);
    myMotor_2->run(FORWARD);
  }
  else if (error_2 <0) {
    myMotor_2->setSpeed(128);
    myMotor_2->run(BACKWARD);
  } 
}
