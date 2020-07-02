#include "prescaler.h"
#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( 2,4,8,7,6,5);
byte ohm[] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01010,
  B11011,
  B00000
};
void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, ohm);
  lcd.home();
  lcd.print("measuring...");
  // put your setup code here, to run once:
  setClockPrescaler(CLOCK_PRESCALER_1);
  pinMode(11, OUTPUT); // Set pin 10 as output
  Serial.begin(9600);

  // put your main code here, to run repeatedly:
  double V = analogRead(A4)/1023.0*5;
  analogWrite(3, 4); 
  trueDelay(1000); 
  double V1 = 0;
  double I1 = 0;
  for(int i = 0;i<1000;i++){      //Find the average of 1000 current and voltage samples
    V1+=analogRead(A4)/1023.0*5; 
    I1+=analogRead(A5)/1023.0*5;
    trueDelay(1);
  }
  V1/=1000;
  I1/=1000;
  String result = "V1: ";
  analogWrite(3, 130);
  trueDelay(1000);
  double V2 = 0;
  double I2 = 0;
  for(int i = 0;i<1000;i++){
    V2+=analogRead(A4)/1023.0*5;
    I2+=analogRead(A5)/1023.0*5;
    trueDelay(1);
  }
  V2/=1000;
  I2/=1000;
  trueDelay(1000);
  Serial.print("I2: ");
  Serial.println(I2 *1000);
  Serial.print("I1: ");
  Serial.println(I1 *1000);
  double r = -(V2-V1)/(I2-I1)*1000;
  analogWrite(3, 0);
  trueDelay(20);
  
  lcd.clear();
  lcd.home();
  lcd.print("Voltage: ");
  lcd.print(V);
  lcd.print("V");
  lcd.setCursor(0,1);
  lcd.print("r:  ");
  lcd.print(r);
  lcd.print("m");
  lcd.write(byte(0));
  

  
}

void loop() {
 
  
}
