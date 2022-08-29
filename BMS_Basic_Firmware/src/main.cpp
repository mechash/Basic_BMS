#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

//Define input pin for sensor and switch

#define Vin A0
#define SW A1

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled_display(U8G2_R0, U8X8_PIN_NONE);

/* Timig or Stopwatch Section*/
long Day=0;
int Hour =0;
int Minute=0;
int Second=0;
int HighMillis=0;
int Rollover=0;

int SW_Select_data_1=0;
int SW_Select_1 = 0;

void uptime(int x){
  unsigned long currentMillis = millis()*x;
  if(currentMillis>=3000000000){ 
    HighMillis=1;
  }
  if(currentMillis<=100000&&HighMillis==1){
    Rollover++;
    HighMillis=0;
  }
  long secsUp = currentMillis/1000;
  Second = secsUp%60;
  Minute = (secsUp/60)%60;
  Hour = (secsUp/(60*60))%24;
  Day = (Rollover*50)+(secsUp/(60*60*24));  //First portion takes care of a rollover [around 50 days]
}



float Vraw = 0.0, Volt_sum = 0.0, Volt_cal = 0.0;
float Volt = 0.0, mVolt = 0.0;
int raw;
float Vref = 5.0, resolution = 1024.0;
float R1 = 5100, R2 = 10000;

float offset = -0.15;

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



float amperage_meter_scale = 410.4488889; 
float amperage_meter_offset = 0.0;
float Amps = 0.0, mah = 0.00;
int mAmps;

unsigned long previousMillis = 0; // Previous time in ms
unsigned long millisPassed = 0;  // Current time in ms

void Cal_Amps(){
  millisPassed = millis() - previousMillis;
  Amps = ( Volt /  amperage_meter_scale * 10.0  + amperage_meter_offset  / 1000.0 );
  mAmps = Amps * 1000.0;
  
  mah = mah + mAmps * (millisPassed / 3600000.0);
  previousMillis = millis();
}


float bat_min = 3.25, bat_max = 4.35, bat_nom = 3.80;
float bat_multi, mAh_cal, mAh_sum;
float cap_charged = 320;
int mAh_offset = 10;
int cap_max, mah_avg;

void Bat_Multi(){
  float bat_diff = bat_max - bat_min;
  float nom_bat_diff = bat_max - bat_nom;
  bat_multi = (cap_charged/1000)/(nom_bat_diff);
  cap_max = ( ( bat_diff * bat_multi ) * 1000 ) + mAh_offset;
}


int dump;
int sum_mAh,avg_mAh ;
int new_mAh ;

void Battery(){
  Bat_Multi();
  mAh_cal= ( ( bat_max - Volt ) * bat_multi ) * 1000;
  mAh_sum += mAh_cal;
  if(dump == 10){
    mah_avg = (mAh_sum / 10) + mAh_offset;
    mAh_sum = 0;
    dump = 0;
  } 
  dump++;
}

unsigned long previousMillis2 = 0;
unsigned long home_page_millis =0;


// home page calling other important function and displaying main results 
void home_page(){
  if(Volt > 1)
  {
    uptime(1);
  }
  else{
    uptime(0);
  
  }
  Cal_Voltage();
  Cal_Amps();
  Battery();
  oled_display.clearBuffer();
  oled_display.setCursor(0,10);	
  oled_display.print(Day);
  oled_display.print(F(" D "));
  oled_display.print(Hour);
  oled_display.print(F(" H "));
  oled_display.print(Minute);
  oled_display.print(F(" M "));
  oled_display.print(Second);
  oled_display.print(F(" S "));
  oled_display.setCursor(0,25);	
  oled_display.print(F("Volt = "));
  oled_display.print(Volt);
  oled_display.print(F(" V ("));
  oled_display.print(Vraw);
  oled_display.print(F(")"));
  oled_display.setCursor(0,38);	
  oled_display.print(F("mAmps = "));
  oled_display.print(mAmps);
  oled_display.print(F(" mA"));
  oled_display.setCursor(0,51);
  oled_display.print(F("mAh = "));
  oled_display.print(mah_avg);
  oled_display.print(F(" mAh ("));
  oled_display.print(cap_max);
  oled_display.print(F(")"));  

  unsigned long currentMillis = millis();
  if(currentMillis - home_page_millis  >= 500) {
    oled_display.sendBuffer();
    home_page_millis = currentMillis;
  }

}


void battery_setting_function(int x){
  
  if (x == 2)
  {
    oled_display.setCursor(0,22);
    oled_display.print(">");
  }
  else if (x == 3)
  {
    oled_display.setCursor(0,34);
    oled_display.print(">");
  }
  else if (x == 4)
  {
    oled_display.setCursor(0,46);
    oled_display.print(">");
  }
  else if (x == 5)
  {
    oled_display.setCursor(10,62);
    oled_display.print(">");
  }
  else if (x == 6)
  {
    oled_display.setCursor(65,62);
    oled_display.print(">");
  }
  
  
  
}

void battery_setting(){
  oled_display.setCursor(25,10);
  oled_display.print("Battery Setting");

  oled_display.setCursor(10,22);
  oled_display.print(F("V-min : "));
  oled_display.print(bat_min);

  oled_display.setCursor(10,34);
  oled_display.print(F("V-max : "));
  oled_display.print(bat_max);


  oled_display.setCursor(10,46);
  oled_display.print(F("V-nom : "));
  oled_display.print(bat_nom);

  oled_display.setCursor(20,62);
  oled_display.print("Save");

  oled_display.setCursor(75,62);
  oled_display.print("Reset");
}


void mAh_setting_function(int x){
  
  if (x == 8)
  {
    oled_display.setCursor(0,22);
    oled_display.print(">");
  }
  else if (x == 9)
  {
    oled_display.setCursor(0,34);
    oled_display.print(">");
  }
  else if (x == 10)
  {
    oled_display.setCursor(0,46);
    oled_display.print(">");
  }
  else if (x == 11)
  {
    oled_display.setCursor(10,62);
    oled_display.print(">");
  }
  else if (x == 12)
  {
    oled_display.setCursor(65,62);
    oled_display.print(">");
  }
  
  
  
}
//asdjbafb

void mAh_setting(){
  oled_display.setCursor(25,10);
  oled_display.print("mAh Setting");

  oled_display.setCursor(10,22);
  oled_display.print(F("Charged : "));
  oled_display.print(cap_charged);

  oled_display.setCursor(10,34);
  oled_display.print(F("Capicity : "));
  oled_display.print(mah_avg);


  oled_display.setCursor(10,46);
  oled_display.print(F("Offset : "));
  oled_display.print(mAh_offset);

  oled_display.setCursor(20,62);
  oled_display.print("Save");

  oled_display.setCursor(75,62);
  oled_display.print("Reset");
}


void voltage_setting_function(int x){
  
  if (x == 14)
  {
    oled_display.setCursor(0,22);
    oled_display.print(">");
  }
  else if (x == 15)
  {
    oled_display.setCursor(0,34);
    oled_display.print(">");
  }
  else if (x == 16)
  {
    oled_display.setCursor(0,46);
    oled_display.print(">");
  }
  else if (x == 17)
  {
    oled_display.setCursor(10,62);
    oled_display.print(">");
  }
  else if (x == 18)
  {
    oled_display.setCursor(65,62);
    oled_display.print(">");
  }
  
  
  
}

void voltage_setting(){
  oled_display.clearBuffer();
  oled_display.setCursor(25,10);
  oled_display.print("Voltage Setting");

  oled_display.setCursor(10,22);
  oled_display.print(F("Volts : "));
  oled_display.print(Volt);
  oled_display.print(F(" V"));

  oled_display.setCursor(10,34);
  oled_display.print(F("Offset : "));
  oled_display.print(offset);
  oled_display.print(F(" V"));

  oled_display.setCursor(20,62);
  oled_display.print("Save");

  oled_display.setCursor(75,62);
  oled_display.print("Reset");

  // unsigned long currentMillis = millis();
  // if(currentMillis - Sub_loop  >= 500) {
  //   oled_display.sendBuffer();
  //   Sub_loop = currentMillis;
  // }
}


int y;
void setting_page(int x){
  Cal_Voltage();
  Cal_Amps();
  Battery();
  y = x;
  
  oled_display.clearBuffer();

  if ( y > 0 && y < 7)
  {
    battery_setting();
    battery_setting_function(y);
  }
  else if (y >6 && y <13)
  {
    mAh_setting();
    mAh_setting_function(y);
  }
  else if (y >12 && y<20)
  {
    voltage_setting();
    voltage_setting_function(y);
  }
  
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis2  >= 400) {
    oled_display.sendBuffer();
    previousMillis2 = currentMillis;
  }
  


}


int SW_OK;
int SW_OK_data;
unsigned long SW_Millis_2 = 0;
unsigned long Sub_loop = 0;
int dumpp=0;

// Menu page function
int SW_Select_data_2 = 0;


int menu_page_function(){
  
  SW_OK = analogRead(SW);

  unsigned long currentMillis = millis();
  if(currentMillis - SW_Millis_2  >= 600) {
    if (SW_OK >= 700 && SW_OK <= 750){
      SW_Select_data_2++;
    }
    if (SW_OK >= 130 && SW_OK <= 180){
      SW_OK_data++;
    }
    SW_Millis_2 = currentMillis;
  }

  if (SW_Select_data_2==1)
  {
    // while (SW_OK_data == 1)
    // {
    //   voltage_setting();
      
    // }
    
    oled_display.setCursor(0,24);
    oled_display.print(">");
    SW_OK_data = 0;
    dumpp =0;
    
  }
  else if (SW_Select_data_2==2)
  {
    oled_display.setCursor(0,36);
    oled_display.print(">");
    SW_OK_data = 0;
    dumpp =0;
  }
  
  else if (SW_Select_data_2==3)
  {
    oled_display.setCursor(0,48);
    oled_display.print(">");
    SW_OK_data = 0;
    dumpp =0;
  }
  
  else if (SW_Select_data_2==4)
  {
    oled_display.setCursor(0,60);
    oled_display.print(">");
    if(SW_OK_data ==1){
      dumpp = 1;      
    }    
  }
  else if (SW_Select_data_2 == 5)
  {
    SW_Select_data_2 = 1;
    dumpp =0;
  }
  else{
    SW_OK_data = 0;
    dumpp =0;
  }
  return dumpp;
}

unsigned long menu_loop_millis = 0;
// Menu page settup
void menu_page(){
  oled_display.clearBuffer();

  menu_page_function();
  
  oled_display.setCursor(55,8);
  oled_display.print("Menu");

  oled_display.setCursor(10,24);
  oled_display.print("Voltage Calibration");

  oled_display.setCursor(10,36);
  oled_display.print("Battery Voltage");

  oled_display.setCursor(10,48);
  oled_display.print("Battery Capacity");

  oled_display.setCursor(10,60);
  oled_display.print("Go Back");
  
  unsigned long currentMillis = millis();
  if(currentMillis - menu_loop_millis  >= 500) {
    oled_display.sendBuffer();
    menu_loop_millis = currentMillis;
  }

  

}


void setup() {
  pinMode(Vin,INPUT);
  pinMode(SW, INPUT);
  Serial.begin(9600);
  oled_display.setI2CAddress(0x78);
  oled_display.begin();
  oled_display.setFont(u8g2_font_profont12_tf);
  oled_display.setCursor(40,34);
  oled_display.print("Welcome");
  oled_display.sendBuffer();
  //delay(1000);
  oled_display.clearDisplay();

  
}


unsigned long SW_Millis_1 = 0;

int SW_data =0;
int SW_raw=0;
void loop() {
  SW_raw = analogRead(SW);

  unsigned long currentMillis = millis();
  if(currentMillis - SW_Millis_1  >= 600) {
    SW_Millis_1 = currentMillis;
    if (SW_raw >= 700 && SW_raw <= 750)
    {
      SW_data++;
    }
  }

  while (SW_data > 1){
    int null = menu_page_function();
    if (null == 1)
    {
      SW_data = 0;
      break;
    }
    else
    {
      menu_page();
    }
  }
  
  home_page();
  Serial.println(SW_raw);
  Serial.println(SW_data);

}



