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

//encoder variables
 //encoder 1 (with interrupt)
int encoderPin1 = 3;
int encoderPin2 = 2;

int MSB_1 = 0;
int LSB_1 = 0;

volatile int lastEncoded_1 = 0;
volatile long encoderValue_1 = 0;
long lastencoderValue_1 = 0;
int lastMSB_1 = 0;
int lastLSB_1 = 0;


 //encoder 2 (without interrupt)
int encoderPin3 = 7;
int encoderPin4 = 6;

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
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(encoderPin3, INPUT);
  pinMode(encoderPin4, INPUT);
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);
  digitalWrite(encoderPin3, HIGH);
  digitalWrite(encoderPin4, HIGH);
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder_1, CHANGE); 
  attachInterrupt(1, updateEncoder_1, CHANGE);
  AFMS.begin();
}

void loop(){
  currentMSB_2 = digitalRead(encoderPin3);
  currentLSB_2 = digitalRead(encoderPin4);
  if (currentMSB_2 != lastMSB_2 || currentLSB_2 !=lastLSB_2){
    updateEncoder_2();
  }
  lastMSB_2 = currentMSB_2;
  lastLSB_2 = currentLSB_2;
  if (millis()-time >= 1000){
    Serial.print("Encodervalue1 is ");
    Serial.println(encoderValue_1);
    Serial.print("Encodervalue2 is ");
    Serial.println(encoderValue_2);
    time = millis();
  }
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
