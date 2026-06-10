#include <Arduino.h>
#include <AFMotor.h>

// M1 ve M2 portlarına bağlı motorları tanımlıyoruz
AF_DCMotor motor1(1); 
AF_DCMotor motor2(2); 

void setup() {
  Serial.begin(9600);           
  Serial.println("Motor Testi Basliyor!");

  // Motorların hızını ayarlıyoruz (0 ile 255 arasında bir değer)
  motor1.setSpeed(200); 
  motor2.setSpeed(200); 

  // Motorları başlangıçta durduruyoruz
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void loop() {
  Serial.println("İleri Donus");
  // Motorları ileri yönde döndür
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  delay(3000); // 3 saniye bekle

  Serial.println("Motorlar Duruyor");
  // Motorları durdur
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  delay(1000); // 1 saniye bekle

  Serial.println("Geri Donus");
  // Motorları geri yönde döndür
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  delay(3000); // 3 saniye bekle

  Serial.println("Motorlar Duruyor");
  // Motorları tekrar durdur
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  delay(2000); // 2 saniye bekle
}

