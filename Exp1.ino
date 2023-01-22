

#include <Wire.h> 
#include <MPU9250_WE.h>                           // gyroscope lybrary (measuring angles)
#include <LiquidCrystal_I2C.h>                    // LCD display library
#include "HX711.h"                                // measuring weight sensor library

#define MPU9250_ADDR 0x68                         // i2c address of the MPU-9250

const int LOADCELL_DOUT_PIN = 3;                  // HX711 circuit wiring
const int LOADCELL_SCK_PIN = 2;

float force;

HX711 scale;
LiquidCrystal_I2C lcd(0x3F, 16, 2);               // i2c address of the LCD could be 0x3f or 0x27 
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

void LCD_Initializing()
{
  lcd.clear();
  lcd.setCursor(0,0);     
  lcd.print("Initializing the");
  lcd.setCursor(0,1); 
  lcd.print("experiment ...");
}

void LCD_Print_Measurement(float Force, float Angle)
{
  float f = Force;  if (f < 0) f = 0;
  float x = Angle;  if (x < 0) x = 0;

  lcd.clear();
  lcd.setCursor(0,0);     
  lcd.print("Force: "); 
  if (f > 1000) {
    f = f/1000; 
    lcd.print(f); lcd.print("N");
  }  else {
    lcd.print(f); lcd.print("mN");
  }
  lcd.setCursor(0,1); 
  lcd.print("Angle: "); lcd.print(x); lcd.print(char(223));
}

// The board setup
void setup() {                          
  //initialize serial communication
  Serial.begin(115200);

  //initialize lcd screen
  Serial.println("Initializing the LCD");
  lcd.init();                 //Init the LCD
  lcd.backlight();            //Activate backlight
  LCD_Initializing();

  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(850.f);       // this value was obtained by calibrating the scale with known weights;
  scale.tare();				          // reset the scale to 0
  scale.read_average(20);


  Serial.println("Initializing the MPU9250");
  Wire.begin();
  if(!myMPU9250.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected. Calibrationg ...");
  }
  myMPU9250.autoOffsets();
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);  

  LCD_Print_Measurement(0, 0);
  Serial.println("nitializing completed!");
}

// the loop function runs over and over again forever
void loop() {
  force = scale.get_units(5) * 9.8;
  xyzFloat angle = myMPU9250.getAngles();
  LCD_Print_Measurement(force, angle.x);
  delay(50);                       // wait for few milliceconds
}
