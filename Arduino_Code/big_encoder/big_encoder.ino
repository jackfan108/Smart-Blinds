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
  pinMode(encoderPin3, INPUT);
  pinMode(encoderPin4, INPUT);
  //digitalWrite(encoderPin3, HIGH);
  //digitalWrite(encoderPin4, HIGH);
}

void loop(){
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
  if (currentMSB_2 != lastMSB_2 || currentLSB_2 !=lastLSB_2){
    updateEncoder_2(currentMSB_2, currentLSB_2);
  }
  lastMSB_2 = currentMSB_2;
  lastLSB_2 = currentLSB_2;
  if (millis()-time >= 1000){
    Serial.print("Encodervalue is ");
    Serial.println(encoderValue_2);
    time = millis();
  }
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
