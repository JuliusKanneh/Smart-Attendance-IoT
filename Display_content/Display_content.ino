#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

 String messageSignal;
 String message;

void setup() 
{  
  // Open serial communications and wait for port to open:
  lcd.begin(16, 2);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  lcd.print("Swap Card!");
  
}

void loop() 
{ 
  delay(4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Swap Card!");
  
  if(Serial.available()) {
    messageSignal = Serial.readString();
    
    if(messageSignal == "successfull"){
      Serial.println("successfull");
      message="Attendance Taken";
    }else if(messageSignal == "not_found"){
      Serial.println("Card not found");
       message="Not Registered";
    }else if(messageSignal == "duplicate"){
      Serial.println("duplicate");
       message="Already Taken";
    }else{
      Serial.println("Unkown_error");
      message="Contact Admin";
     }

    delay(4);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println(message);
    Serial.println(message);
    delay(3000);
  }
  delay(1000);
}
