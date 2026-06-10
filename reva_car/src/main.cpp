#include <Arduino.h>
#include <AFMotor.h>

// M1 ve M2 portlarına bağlı motorları tanımlıyoruz
AF_DCMotor motor1(1); 
AF_DCMotor motor2(2); 

void setup() {
  Serial.begin(9600);           
  
  // Motorların hızını ayarlıyoruz (0 ile 255 arasında bir değer)
  motor1.setSpeed(200); 
  motor2.setSpeed(200); 

  // Motorları başlangıçta durduruyoruz
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void loop() {
  // Kontrol kodları buraya eklenebilir
}


