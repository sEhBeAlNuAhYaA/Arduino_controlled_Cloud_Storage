#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

using namespace std;

byte slash[8] = {
	0b00000,
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00000,
	0b00000
};
byte load[8]={
  0b11111,
	0b00000,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b00000,
	0b11111
};
byte load_not[8]={
  0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111
};
byte load_open[8]={
  0b11111,
	0b00000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b00000,
	0b11111
};
byte load_close[8]={
  0b11111,
	0b00000,
	0b00001,
	0b00001,
	0b00001,
	0b00001,
	0b00000,
	0b11111
};

class DisplayInfo{

  int current_client;
  String current_connection_info;
  String current_rep_info;
  String prev_message;

  public:
    DisplayInfo(){
      this->current_client = 0;
      this->current_connection_info = "00";
      this->current_rep_info = "00";
      this->prev_message = "00";
      PRINT_CONNECTION();
    }
    void Update_Display(){
      if(!Catch_connection()) return;
      if(this->current_client != 0){
        PRINT_STORAGE();
      }else{
        PRINT_CONNECTION();
      }
    }
    bool Catch_connection(){
      String con_info = "";
      
      if(Serial.available()){
        con_info = Serial.readString();
        if(con_info.equals(this->prev_message)){
          return 0;
        }
        if(con_info.length() == 2){
          this->current_connection_info = con_info;
          this->current_client = 0;
          this->prev_message = con_info;
          return 1;
        } 
       if(con_info.length() == 3){
          this->current_client = con_info.substring(0,1).toInt();
          this->current_rep_info = con_info.substring(1,3);
          this->prev_message = con_info;
          return 1;
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(con_info);
        lcd.setCursor(0,1);
        lcd.print(con_info.length());
        delay(2000);
        ERROR("WRONG SERIAL");
        return 0;
      }else{
        return 0;
      }
    }
    void PRINT_STORAGE(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CLIENT ");
      lcd.print(this->current_client);
      lcd.print(" STORAGE");
      
      lcd.setCursor(0,1);
      for(int i = 0; i < 10;i++){
        if(i == 0){
          lcd.print(char(3));
          continue;
        }
        if(i == 9){
          lcd.print(char(4));
          continue;
        }
        lcd.print(char(2));       
      }

      for(int i = 0; i < 10;i++){
        if(i < current_rep_info.toInt()/10){
          lcd.setCursor(11,1);
          lcd.print(i*10);
          lcd.print("%");
        }else{
          lcd.setCursor(11,1);
          lcd.print(current_rep_info.toInt());
          lcd.print("%");
        }
        lcd.setCursor(i,1);
        if(current_rep_info.toInt()/10 == 0 && i == 0){
            lcd.print(char(3));
            delay(300);
          }
        if(i < current_rep_info.toInt()/10){
            lcd.print(char(1));
            delay(300);
        }else{
          if(current_rep_info.toInt() >= 95 && i == 9){
            lcd.print(char(1));
            delay(300);
          }else if(i!=9){
            lcd.print(char(2));
            delay(300);
          }else{
            lcd.print(char(4));
            delay(300);
          }
        }
        
      }

      lcd.setCursor(11,1);
      if(current_rep_info.toInt() >= 95){
        lcd.print("FULL");
      }
    }
    void PRINT_CONNECTION(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("COUNT OF CLIENTS:");
      lcd.setCursor(0,1);
      lcd.print(this->current_connection_info);
    }
    void ERROR(String error){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(error);
      lcd.setCursor(0,1);
      lcd.print("RESTART DEVICE");
      //delay(10000);
    }
};

DisplayInfo* disp;

void setup(){
  Serial.begin(57600);
 
  lcd.init();                     
  lcd.backlight();
  disp = new DisplayInfo();

  lcd.createChar(0,slash);
  lcd.createChar(1,load);
  lcd.createChar(2,load_not);
  lcd.createChar(3,load_open);
  lcd.createChar(4,load_close);
  
}
bool isitconnected = false;
void loop(){
  if (isitconnected == false){
    if(Serial.available()){
      char open_buffer[2];
      Serial.readBytes(open_buffer,2);
      isitconnected = true;
    }
  }else{
    disp->Update_Display();
  }
  
}
