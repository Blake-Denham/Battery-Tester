//--------------------------------------------------------------------------------------
//                                         INCLUDES
//--------------------------------------------------------------------------------------
#include <MCP23S17_LCD.h>
#include <SPI.h>
#include "Wire.h"
#include "MCP7941x.h"
MCP23S17_LCD lcd(A3, 10, PORT_B);

//--------------------------------------------------------------------------------------
//                                        DEFINES
//--------------------------------------------------------------------------------------
#define OLED_RESET  4                       // OLED reset pin
#define vIn    A0                           // raw battery voltage
#define tacSwitch   5                       // tac switch to activate load
#define xBase       9                       // xsistor base pin

//--------------------------------------------------------------------------------------
//                                       VARIABLES
//--------------------------------------------------------------------------------------
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
float v1 = 0.000;     // unloaded battery voltage
float v2 = 0.000;     // loaded battery voltage
float i1 = 0.000;     // unloaded current
float i2 = 0.000;     // loaded current
float intRes = 0.000; // battery internal resistance in Ohms
float vCal = 5;       // calculated vRef (measure between 5V & Gnd)
int load = 5;         // load resistance in Ohms
bool j = 1;           // is switch triggered

int battSelect = 0;
int numBatts = 4;
String battTypes[4] = {"Duracell AA     ","Energizer AA    ","Duracell AAA    ","Duracell Coin   "};
bool state = 0;       //which menu you are in

int sampleNum = 10;   // how many times the r is calcualted and the averaged
//// CAPACITY
int capacity = 0;
int capacitypercent = 0;
struct Battery {
  float vH;
  float vL;
  int MAH;
};
//Create new batteries
Battery duracellAA = {1.6, 0.8, 2200}; //vH, vL, MAH
//got these values from datasheet MN1500 and page 25 of notes
Battery energizerAA = {1.6, 0.8, 1750}; //vH, vL, MAH
//got these values from datasheet E91 and page 26 of notes

Battery duracellAAA = {1.6, 0.8, 890};
//from datasheet mn2400 - got 0,89A for i2 and so looked between 0.5 and 1 mA
Battery duracellCoin = {3, 2, 150};
//from datasheet DL2032, 150 from Mah - 1kohm at 20 degrees C ambient



//way to index all batteries
Battery battList[4] = {duracellAA, energizerAA, duracellAAA, duracellCoin};

void setup()   {

  pinMode(7, OUTPUT); //cs2 - FOR CONTRAST USING DIGIPOT
  SPI.begin();
  delay(10);
  // set the contract with digital potentiometer
  digitalWrite(7, LOW);
  SPI.transfer(0x00);
  SPI.transfer(0x20);
  digitalWrite(7, HIGH);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(0, ohm); //must be after lcd.begin(16,2);
  lcd.setCursor(0, 0);
  //lcd.clear();
  Serial.begin(9600);

  pinMode(vIn, INPUT);                // set A0 as input
  pinMode(tacSwitch, INPUT_PULLUP);   // set D5 as input with internal pullup
  pinMode(xBase, OUTPUT);             // set D9 as output
  v1 = (analogRead(vIn) * vCal) / 1023;   // get unloaded voltage
  i1 = v1 / 1000;                         // calculate unloaded current
  Serial.println("i1");
  Serial.println(i1);

}


void loop() {
  //Serial.print(state);
  //j = digitalRead(tacSwitch);
  /////////////////BATTERY SELECTION//////////////////
  if (state == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Select battery:");
    lcd.setCursor(0, 1);
    lcd.print(battTypes[battSelect]);
    // is switch triggered? 1 = no 0 = yes
    //j = digitalRead(tacSwitch);
    if (digitalRead(tacSwitch) == 0) {
      delay(500);
      //toggle with short press
      if (digitalRead(tacSwitch) == 1) {
        battSelect = battSelect+1;
        if(battSelect==numBatts)
        {
            battSelect = 0;
        }
      }
      //enter menu with long press
      else {
        state = 1;
        capacitycalc(); // must be here to select correct battery - was in setup
        lcd.setCursor(0, 0);  
        //TODO - make a you chose <battery> selection feedback
        lcd.clear();
      }
    }

  }/////////////////BATTERY MEASURMENT//////////////////
  else {
    // is switch triggered? 1 = no 0 = yes
    if (digitalRead(tacSwitch) == 0) {
      ircalcmany(); //IMPORTANT LINE
    }
    //LCD OUTPUT

    lcd.setCursor(0, 0);                  // set cursor upper left
    lcd.print("v");                   // print header info unloaded volts
    lcd.print(v1, 3);                   // print volts to 3 digits
    lcd.print("V");

    lcd.print(" c");                  //Print capacity
    lcd.print(capacity);
    lcd.print("mAh");
    //lcd.print("I1: ");                   // print header info unloaded current
    //lcd.print(i1,3);                    // print current to 3 digits
    //lcd.print("A");
    //lcd.print("V2: ");                   // print header info loaded volts
    //lcd.print(v2,3);                    // print volts to 3 digits
    //lcd.print("V");
    //lcd.print("I2: ");                   // print header info loaded current
    //lcd.print(i2,3);                    // print current to 3 digits
    //lcd.print("A");
    //lcd.print(" ");                     // skip a line

    lcd.setCursor(0, 1);
    lcd.print("r");                     // print header info internal resistance
    lcd.print(intRes, 2);                 // print internal resistance to 3 digits
    lcd.print("m");                    // print unit of measurement
    //lcd.print("ohm");
    lcd.write(byte(0));               //custom ohm character

    lcd.print(" c%");                 //print capacity percent
    lcd.print(capacitypercent);
    lcd.print("%");
    //SERIAL OUTPUT FOR MORE INFORMATION/TESTING
    //  Serial.print("  V1  :");                   // print header info unloaded volts
    //  Serial.print(v1,3);                    // print volts to 3 digits
    //  Serial.print("  I1  :");                   // print header info unloaded current
    //  Serial.print(i1,3);                    // print current to 3 digits
    //  Serial.print("  V2  :");                   // print header info loaded volts
    //  Serial.print(v2,3);                    // print volts to 3 digits
    //  Serial.print("  I2  :");                   // print header info loaded current
    //  Serial.print(i2,3);                    // print current to 3 digits
    //  Serial.print(" ");                     // skip a line
    //  //Serial.setCursor(0, 1);
    //  Serial.print("IR:");             // print header info internal resistance
    //  Serial.print(intRes,3);                  // print internal resistance to 3 digits
    //
    //  Serial.print("IR:");
    //  Serial.println(intRes,3);

    delay(500);                               // delay .5 seconds
    //lcd.clear();

  }
}

void ircalc() {                         // resistance calc function
  digitalWrite(xBase, HIGH);            // switch in the load
  v2 = (analogRead(vIn) * vCal) / 1023; // get loaded battery voltage
  i2 = v2 / load;                       // calculate loaded current
  
  digitalWrite(xBase, LOW);            // switch out the load

  /*
                   Formula to Calculate Internal Resistance
                             Using Voltage Drop

                                   V1 - V2
                             Ri =  -------
                                   I2 - I1
  */
  //Serial.println("i2: ");
  //Serial.println(i2);
  float vCalc = (v1 - v2);
  float iCalc = (i2 - i1);
  intRes = 1000 * (vCalc / iCalc) * -1;

}
void ircalcmany() {
  v2 = 0.000;
  i2 = 0.000;
  digitalWrite(xBase, HIGH);// switch in the load
  //delay(100);
  for (int i = 0; i < sampleNum; i++) {     // measure sampleNum times
    v2 +=(analogRead(vIn) * vCal) / 1023; // get sum loaded battery voltage
    delay(10);
  }
  digitalWrite(xBase, LOW);            // switch out the load
  v2 = v2 / sampleNum;                 // get average loaded battery voltage
  i2 = v2 / load;                     // calculate average loaded current
  /*
                   Formula to Calculate Internal Resistance
                             Using Voltage Drop

                                   V1 - V2
                             Ri =  -------
                                   I2 - I1
  */
  Serial.println("i2: ");
  Serial.println(i2);
  float vCalc = (v1 - v2);
  float iCalc = (i2 - i1);
  intRes = 1000 * (vCalc / iCalc) * -1; //*1000 to get in milliohms

}

void capacitycalc()
{
  //Testing
//  Serial.println(battList[battSelect].MAH);
//  Serial.println(v1);
//  Serial.println(battList[battSelect].vH);
//  Serial.println(battList[battSelect].vL);

  //CERTAIN CHARACTERISTICS OF BATTERY MUST BE SELECTED
  float temp = (v1 - battList[battSelect].vL) * (100 / (battList[battSelect].vH - battList[battSelect].vL));
  capacitypercent = temp;
  temp = battList[battSelect].MAH * 1 / 100 * temp;
  capacity = temp;

}
