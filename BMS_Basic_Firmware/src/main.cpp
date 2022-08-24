#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define Vout A0

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled_display(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);

float V1_multiplier = 0.004730594;//0.004217391;//0.004210230;
float V2_multiplier = 1.031250000;

float V1_offset = 0.003;
float V2_offset = -0.003;

int raw = 0.00;
double V1_val = 0.00, V2_val = 0.00;
double V1_sum = 0.00, V2_sum = 0.00;
double V1_avg , V2_avg = 0.00;

String V = " V";

void raw_avg(){
  for(int i = 0; i <= 100; i++)
  {
    raw = analogRead(A0);
    V1_sum = (raw * 5.0)/1024.0;
    V1_avg = 5100+10000;
    V1_val = (10000/V1_avg);
    V2_avg = (V1_sum /V1_val);
    V2_sum += V2_avg;
    if(i == 100)
    {
      V2_val = (V2_sum / 100)+0.02;
      V2_sum = 0.00;
    }
  //   raw = analogRead(Vout);
  //   V1_avg = raw * V1_multiplier;
  //   V1_sum += V1_avg;
  //   V2_avg = (raw * V1_multiplier)*V2_multiplier;
  //   V2_sum += V2_avg;
  //   if(i == 99){
  //     V1_val = V1_sum /99;
  //     V1_sum = 0.00;
  //     V2_val = V2_sum /99;
  //     V2_sum = 0.00;
  //   }
  }
}


// void Calculation(){
//   for(int i = 0; i <= 99; i++)
//   {
//     V1_val = (raw * V1_multiplier) + V1_offset;
//     V2_val = (V1_val * V2_multiplier) + V2_offset;
//     V1_sum += V1_val;
//     V2_sum += V2_val; 
//     if(i == 99)
//     {
//       V1_avg = V1_sum / 100;
//       V2_avg = V2_sum / 100;      
//       V1_sum = 0.00;
//       V2_sum = 0.00;
//     }
//   }
// }

void setup() {

  Serial.begin(9600);
  pinMode(Vout,INPUT);
  oled_display.setI2CAddress(0x78);
  oled_display.begin();
  oled_display.setFont(u8g2_font_profont12_tf);
  oled_display.clearDisplay();
  
}

void loop() {
  raw_avg();
  //Calculation();
  oled_display.clearBuffer();					// clear the internal memory
  oled_display.setCursor(0,8);	
  oled_display.print("raw = ");
  oled_display.setCursor(35,8);
  oled_display.print(raw);
  oled_display.setCursor(0,18);	
  oled_display.print("V1  = ");
  oled_display.setCursor(35,18);
  oled_display.print(V1_val + V);
  oled_display.setCursor(0,28);	
  oled_display.print("V2 = ");
  oled_display.setCursor(35,28);
  oled_display.print(V2_val + V);
  oled_display.sendBuffer();					// transfer internal memory to the display
  delay(100);  
}
