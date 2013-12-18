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

#define WLAN_SSID       "SFwireless"           // cannot be longer than 32 characters!
#define WLAN_PASS       "9094683070"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

byte ip[4] = {183, 10, 168, 192};
uint32_t *ip4 = (uint32_t*)&ip;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);

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


long update_period = 1000;
long last_update = 0;
bool connecting = false;
Adafruit_CC3000_Client tcp;

void loop() {
  int value = analogRead(A0);

  if ((millis() > (last_update + update_period)) &&
      (!connecting)) {
    connecting = true;
    /* Try connecting to the website */
    tcp = cc3000.connectTCP(*ip4, 9999);
  }

  if (connecting && tcp.connected()) {
    String s = String(analogRead(A0));
    char buf[8];
    s.toCharArray(buf, 8);
    
    tcp.fastrprint("GET /set?position=");
    tcp.fastrprint(buf);
    tcp.fastrprint(" HTTP/1.0\r\nr\n");
    
    while (tcp.connected()) { tcp.read(); }
    tcp.close();
    connecting = false;
    last_update = millis();
  }
}
