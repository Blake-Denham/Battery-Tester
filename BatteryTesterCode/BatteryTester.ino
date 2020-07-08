#include <LiquidCrystal.h>
#include "prescaler.h"

#define controlPin 9
#define negPowerPin 10
#define batteryInput1 1
#define batteryInput2 2


byte ohmChar = 0;

LiquidCrystal lcd( 8,7, 6, 5, 4, 3);
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
  setClockPrescaler(CLOCK_PRESCALER_1);
  Serial.begin(9600);
  pinMode(controlPin, OUTPUT);
  pinMode(negPowerPin, OUTPUT);
  pinMode(batteryInput1, INPUT);
  pinMode(batteryInput2, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.createChar(0, ohm);
  lcd.home();
  analogWrite(controlPin,0);  
 // attachInterrupt(digitalPinToInterrupt(batteryInput1), onBatteryConnect, RISING);
  attachInterrupt(digitalPinToInterrupt(batteryInput2), onBatteryConnect, FALLING);
}
bool testing =false;
void onBatteryConnect(){
  testing = true;
  
}

void loop() {
  if(testing){
    lcd.clear();
    lcd.home();
    lcd.print("measuring.");
  double V = analogRead(A4)/1023.0*5;
  setCurrent(5/1000);
  trueDelay(10); 
  double V1 = 0;
  double I1 = 0;
  for(int i = 0;i<1000;i++){      //Find the average of 1000 current and voltage samples
    V1+=analogRead(A4)/1023.0*5; 
    I1+=analogRead(A5)/1023.0*5;
    trueDelay(1);
  }
   lcd.clear();
    lcd.home();
    lcd.print("measuring..");
  V1/=1000;
  I1/=1000;
  String result = "V1: ";
  setCurrent(0.505);
  trueDelay(1000);
  double V2 = 0;
  double I2 = 0;
  for(int i = 0;i<1000;i++){
    V2+=analogRead(A4)/1023.0*5;
    I2+=analogRead(A5)/1023.0*5;
    trueDelay(1);
  }
   lcd.clear();
    lcd.home();
    lcd.print("measuring...");
  V2/=1000;
  I2/=1000;
  trueDelay(1000);
  Serial.print("I2: ");
  Serial.println(I2 *1000);
  Serial.print("I1: ");
  Serial.println(I1 *1000);
  double r = -(V2-V1)/(I2-I1)*1000;
  setCurrent(0);
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
  testing = false;
  }
}

void setCurrent(float current){
  if(current >1) current=1;
  else if (current <0) current=0;
  int val = current*255;
  analogWrite(controlPin,val);
}
