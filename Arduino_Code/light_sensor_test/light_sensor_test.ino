int analog_input = A0;
int sensor_value = 0;

void setup(){
  pinMode(analog_input, INPUT);
  Serial.begin(9600);
}

void loop(){
  sensor_value = analogRead(analog_input);
  Serial.println(sensor_value);
  delay(100);
}
