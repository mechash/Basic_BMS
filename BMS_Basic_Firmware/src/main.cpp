#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define Vin A0

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled_display(U8G2_R0, U8X8_PIN_NONE);

int raw;
float Vref = 5.0, resolution = 1024.0;
float R1 = 5100, R2 = 10000;

float offset = -0.04;

float Vraw = 0.0, Volt_sum = 0.0, Volt_cal = 0.0;
float Volt = 0.0, mVolt = 0.0;

float amperage_meter_scale = 410.4488889; 
float amperage_meter_offset = 0.0;
float Amps = 0.0, mAmps = 0.0, mah = 0.00;

unsigned long previousMillis = 0; // Previous time in ms
unsigned long millisPassed = 0;  // Current time in ms

int a;
int sum_mAh,avg_mAh ;
int new_mAh ;

String V = "V";
String mV = "mV";
String Amp = "A";
String mA = "mA";
String mAh = "mAh";
String V_ch, mA_ch, mAh_ch ;

void Cal_Voltage(){
  for( int i = 0; i <= 100; i++ )
  {
    raw = analogRead(Vin);
    Vraw = ( ( raw * Vref ) / resolution ) - 0.1;
    Volt_cal = ( Vraw / ( R2 / ( R1 + R2 ) ) );
    Volt_sum += Volt_cal;
    if( i == 100 )
    {
      Volt = ( Volt_sum / 100 ) + offset;
      mVolt = Volt * 1000;
      Volt_sum = 0.00;
    }
  }
}


void Cal_Amps(){
  millisPassed = millis() - previousMillis;
  Amps = ( Volt /  amperage_meter_scale * 10.0  + amperage_meter_offset  / 1000.0 );
  mAmps = Amps * 1000.0;
  
  mah = mah + mAmps * (millisPassed / 3600000.0);
  previousMillis = millis();
}

void Cal_mAh(){
  float set_mult = 0.581818182;
  sum_mAh = ((4.35 - Volt) * set_mult)*1000;
  avg_mAh += sum_mAh;
  a ++;

  if(a == 10){
    new_mAh = avg_mAh / 10;
    mAh_ch = new_mAh + mAh ;
    a = 0;
    avg_mAh = 0;
  }
}

void display_print(){
  oled_display.clearBuffer();					// clear the internal memory
  V_ch = Volt + V;
  oled_display.setCursor(0,20);	
  oled_display.print(F("Volt = "));
  oled_display.print(Volt);
  oled_display.print(F(" V"));
  oled_display.setCursor(0,30);	
  oled_display.print(F("mAmps = "));
  oled_display.print(Amps);
  oled_display.print(F(" mA"));
  // oled_display.setCursor(35,18);
  // oled_display.print(Volt );
  // oled_display.setCursor(0,28);	
  // oled_display.print("mV  = ");
  // oled_display.setCursor(35,28);
  // oled_display.print(mVolt );
  // oled_display.setCursor(0,38);
  // oled_display.print("mAmp = ");
  // oled_display.setCursor(40,38);
  // oled_display.print(mAmps );
  //oled_display.setCursor(0,48);
  // oled_display.print("mAh = ");
  oled_display.setCursor(0,50);
  oled_display.print(F("mAh = "));
  oled_display.print(new_mAh);
  oled_display.print(F(" mAh"));
  oled_display.sendBuffer();	
}

void setup() {

  Serial.begin(9600);
  pinMode(Vin,INPUT);
  oled_display.setI2CAddress(0x78);
  oled_display.begin();
  oled_display.setFont(u8g2_font_profont12_tf);
  oled_display.clearDisplay();
  
}
long interval = 1000;

unsigned long previousMillis2 = 0;
void loop() {
  Cal_Voltage();
  Cal_mAh();
  Cal_Amps();

unsigned long currentMillis2 = millis();
  
  
  if(currentMillis2 - previousMillis2  >= 1000) {
    
    // save the last time you blinked the LED 
    
    display_print();
    previousMillis2 = currentMillis2;
    
  }
   
}
