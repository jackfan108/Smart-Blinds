int tempSensor = A1;

//These variables keep track of temperature
//They need to be longs because of the conversion process
long temperature; //Temperature to print
long temperatureC; //Temperature in degrees C

void setup() {
//inputs are sensor and button to switch between modes
pinMode(tempSensor, INPUT);
Serial.begin(9600);
}

void loop() {
      tempC();
 
}

void tempC(){ //convert reading to temperature in degrees C
  long tempRead = analogRead(tempSensor);
  temperatureC = ((tempRead*5000)/1024) - 500 ; //conversion equation
  temperature = temperatureC; //print it in Celsius
  Serial.println(temperature);
}

void tempF(){ //convert reading to temperature in degrees F
  long tempRead = analogRead(tempSensor);
  temperatureC = ((tempRead*5000)/1024) - 500; //Calculate Celsius
  temperature = (temperatureC*9)/5 + 320; //Convert from C to F
  Serial.println(temperature);
}
