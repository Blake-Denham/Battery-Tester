#include "prescaler.h"
#include <LiquidCrystal.h>
//LCD pin to Arduino
const int currentPin =9;
const int negPin = 10;
LiquidCrystal lcd( 7,6, 5, 4, 3, 2);
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
   pinMode(negPin, OUTPUT);setClockPrescaler(CLOCK_PRESCALER_1);
  analogWrite(3, 127);
  lcd.begin(16, 2);
  lcd.createChar(0, ohm);
  lcd.home();
  lcd.print("measuring...");
  // put your setup code here, to run once:
  
  pinMode(curretnPin, OUTPUT); // Set pin 10 as output
  pinMode(negPin, OUTPUT);
  
  

  Serial.begin(9600);

  // put your main code here, to run repeatedly:
  double V = analogRead(A4)/1023.0*5;
  analogWrite(currentPin, 4); 
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
  analogWrite(currentPin, 130);
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
  analogWrite(currentPin, 0);
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
