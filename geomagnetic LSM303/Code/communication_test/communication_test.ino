#include <Wire.h>
#include <LSM303.h>
#include <math.h>

LSM303 compass;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
}

void loop() {
  // put your main code here, to run repeatedly:
compass.read();
Serial.print("A ");
  Serial.print("X: ");
  Serial.print((int)ax);
  Serial.print(" Y: ");
  Serial.print((int)ay);
  Serial.print(" Z: ");
 Serial.print((int)az);

  Serial.print(" M ");  
  Serial.print("X: ");
  Serial.print((int)compass.m.x);
  Serial.print(" Y: ");
  Serial.print((int)compass.m.y);
 Serial.print(" Z: ");
  Serial.println((int)compass.m.z);
  
  delay(100);
}
