// Create variables for the Trig and echo pins of the ultrasonic sensor.
//trig pin is connected to digital pin 11 and echo pin is connected to digital pin 12.
int trigPin = D6; 
int echoPin = D5;
/*
 The variable duration holds the time between signal transmission and reception.
 The variable cm will save the distance in centimeters.
 The variable inch will save the distance in inches
*/
long duration, cm, inches;
//setup()Initialize the serial port with baud rate 9600 and set the trig pin to output and the echo pin to input.
void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
//loop()The sensor is triggered by sending a HIGH pulse of 10 microseconds. Previously, a short low pulse is given to ensure that a clean high pulse is obtained.
void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(20000);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(12);
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH);
  /*Calculation formula.
  Distance = (propagation time / 2) x speed of sound
  Speed of sound is: 343m / s = 0.0343cm / uS = 1 / 29.1cm / uS
  */
  cm = (duration/2) / 29.1;
  inches = (duration/2) / 74;
  //Print the results, which are displayed on the serial monitor
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
}
