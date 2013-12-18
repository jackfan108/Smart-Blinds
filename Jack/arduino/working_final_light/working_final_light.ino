#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID       "OLIN_GUEST"           // cannot be longer than 32 characters!
#define WLAN_PASS       "The_Phoenix_Flies"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA

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
//int target_2 = 120;
float error_1 = 0;
//float error_2 = 0;

byte ip[4] = {60, 51, 168, 192};
uint32_t *ip4 = (uint32_t*)&ip;

//light sensor variables
int light_analog_input = A0;
int light_sensor_value = 0;
String light;
//temperature sensor variable
int temp_analog_input = A1;
int temp_sensor_value = 0;
long temperature; //Temperature to print
long temperatureC; //Temperature in degrees C
long temperatureF; //Temperature in degress fahrenheit
String tempF;
String tempC;

//encoder variables
 //encoder 1 (without interrupt)
int encoderPin1 = 7;
int encoderPin2 = 6;

volatile int lastEncoded_1 = 0;
volatile long encoderValue_1 = 0;

long lastencoderValue_1 = 0;
int currentMSB_1 = 0;
int currentLSB_1 = 0;
int lastMSB_1 = 0;
int lastLSB_1 = 0;

Adafruit_CC3000_Client tcp;

void setup() {
  Serial.begin(115200);
  pinMode(light_analog_input, INPUT);
  pinMode(temp_analog_input, INPUT);
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);
  /* Initialise the module */
  Serial.println("\nInitializing WiFi...");
  if (!cc3000.begin()) {
    Serial.println("Couldn't begin()! Check your wiring?");
    while(1);
  }
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println("Failed!");
    while(1);
  }
   
  Serial.println("Connected!");
  
  /* Wait for DHCP to complete */
  Serial.println("Requesting DHCP");
  while (!cc3000.checkDHCP()) {
    delay(500); // ToDo: Insert a DHCP timeout!
  }
  AFMS.begin();
}
char data;
int flag1 = 1;       // This flag is for connecting to the TCP server just once
int testvalue = 1;   // This value is used for testing and increments every second
char buffer [5];
String inputtarget = "";
String datasendstr;
char datasenda[30];
String target01;
String target02;
unsigned long time;
unsigned long time2;

void loop() {
  lightsensor();
  tempsensor();
  if (flag1==1){
    tcp = cc3000.connectTCP(*ip4, 9999);
  }
  //Serial.println(F("a"));
  while(!tcp.connected()){
    Serial.println(F("Not Connected"));
    delay(1000);}
  testvalue += 1;
  datasendstr = "";
  datasendstr += encoderValue_1;
  datasendstr += 'a';
  datasendstr += testvalue;
  datasendstr += 'b';
  datasendstr += tempF;
  datasendstr += 'c';
  datasendstr += tempC;
  datasendstr += 'd';
  datasendstr += light;
  datasendstr += 'e';
  //Serial.print(F("datasendstr = "));
  //Serial.println(datasendstr);
  //Serial.print(datasendstr.length());
  //Serial.print("a");
  datasendstr.toCharArray(datasenda,datasendstr.length());
  //Serial.print(F("data sent"));
  //Serial.println(datasenda);
  //Serial.print("b");
  if (millis() - time2 >= 100){
    tcp.fastrprint(datasenda);
    time2 = millis();
  }
  //tcp.fastrprint(itoa(testvalue,buffer,10)); //changes the int(testvalue) to a string for fastrprint()
  //reads the target position char by char until the buffer is empty
  //Serial.print(F("target position = "));
  inputtarget = "";
  //Serial.print("c");  
  while (tcp.available()){
    data = tcp.read();
    if (data=='a'){
      break;
    }
    //Serial.print(data);
    inputtarget = inputtarget + data;
    target01 = inputtarget;
  }
  //target_1 = target01.toInt();
  if (light_sensor_value <= 20){
    target_1 = 350;
  }
  else {
    target_1 = 0;
  }
    
  inputtarget = "";
  //Serial.print("d");
  while (tcp.available()){
    data = tcp.read();
    if (data =='b'){
      break;
    }
    inputtarget = inputtarget + data;
  }
  //Serial.print("e");
  target02 = inputtarget;
  //Serial.println(F(""));
  //Serial.print(F("target01 = "));
  //Serial.println(target01);
  //Serial.print(F("target02 = "));
  //Serial.println(target02);
  flag1 = 0;
  currentMSB_1 = digitalRead(encoderPin1);
  currentLSB_1 = digitalRead(encoderPin2);
  if (currentMSB_1 != lastMSB_1 || currentLSB_1 !=lastLSB_1){
    updateEncoder_1();
  }
  lastMSB_1 = currentMSB_1;
  lastLSB_1 = currentLSB_1;
  motor_1();
  if (millis() - time >= 1000){
    Serial.print(F("target_1 = "));
    Serial.println(target_1);
    Serial.print(F("encoderValue_1 = "));
    Serial.println(encoderValue_1);
    Serial.print(F("tempF = "));
    Serial.println(tempF);
    Serial.print(F("tempC = "));
    Serial.println(tempC);
    Serial.print(F("light = "));
    Serial.println(light);
    Serial.print(F("datasenda is "));
    Serial.println(datasenda);
    Serial.print(F("error_1 = "));
    Serial.println(error_1);
    time = millis();
  }
}

void lightsensor(){
  light_sensor_value = analogRead(light_analog_input);
  light = String(light_sensor_value,10);
  //Serial.print("Light sensor value = ");
  //Serial.println(light_sensor_value);
}
void tempsensor(){
  long temp_sensor_value = analogRead(temp_analog_input);
  temperatureC = ((temp_sensor_value*5000)/1024) - 500 ; //conversion equation
  temperatureC = temperatureC/10; //print it in Celsius
  tempC = String(temperatureC,10);
  //Serial.print("The temperature in celcius is ");
  //Serial.println(temperatureC);
  temperatureF = (temperatureC*9)/5 + 32; //Convert from C to F
  tempF = String(temperatureF,10);
  //Serial.print("The temperature in Fahrenheit is ");
  //Serial.println(temperatureF);
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

void motor_1(){
  error_1 = target_1 - encoderValue_1;
  if (error_1 <= 2 && error_1 >= -2) {
    //Serial.println("speed set to zero!!!");
    myMotor_1->setSpeed(0);
    myMotor_1->run(FORWARD);
  }
  else if (error_1 > 0) {
    myMotor_1->setSpeed(255);
    myMotor_1->run(FORWARD);
  }
  else if (error_1 < 0) {
    myMotor_1->setSpeed(255);
    myMotor_1->run(BACKWARD);
  } 
}
