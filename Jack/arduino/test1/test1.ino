#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>

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

byte ip[4] = {60, 51, 168, 192};
uint32_t *ip4 = (uint32_t*)&ip;

void setup() {
  Serial.begin(115200);

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
}

Adafruit_CC3000_Client tcp;
char data;
int flag1 = 1;       // This flag is for connecting to the TCP server just once
int testvalue = 1;   // This value is used for testing and increments every second
char buffer [5];
String inputtarget = "";
String datasendstr;
char datasenda[30];
String target01;
String target02;

void loop() {  
  if (flag1==1){
    tcp = cc3000.connectTCP(*ip4, 9999);
  }
  //Serial.println(F("a"));
  while(!tcp.connected()){
    Serial.println(F("Not Connected"));
    delay(1000);}
  testvalue += 1;
  datasendstr = "";
  datasendstr += testvalue;
  datasendstr += 'a';
  datasendstr += testvalue;
  datasendstr += 'b';
  datasendstr += testvalue;
  datasendstr += 'c';
  datasendstr += testvalue;
  datasendstr += 'd';
  datasendstr += testvalue;
  datasendstr += 'e';  
  Serial.print(F("datasendstr = "));
  Serial.println(datasendstr);
  Serial.print(datasendstr.length());
  datasendstr.toCharArray(datasenda,datasendstr.length());
  Serial.print(F("data sent"));
  Serial.println(datasenda);
  tcp.fastrprint(datasenda);
  //tcp.fastrprint(itoa(testvalue,buffer,10)); //changes the int(testvalue) to a string for fastrprint()
  //reads the target position char by char until the buffer is empty
  //Serial.print(F("target position = "));
  inputtarget = "";
  while (tcp.available()){
    data = tcp.read();
    if (data=='a'){
      break;
    }
    //Serial.print(data);
    inputtarget = inputtarget + data;
  }
  target01 = inputtarget;
  inputtarget = "";
  while (tcp.available()){
    data = tcp.read();
    if (data =='b'){
      break;
    }
    inputtarget = inputtarget + data;
  }
  target02 = inputtarget;
  //Serial.println(F(""));
  Serial.print(F("target01 = "));
  Serial.println(target01);
  Serial.print(F("target02 = "));
  Serial.println(target02);
  flag1 = 0;
}
