#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const char* ssid = "GEEKS-TEAM";
const char* password = "com.keypass@rw-lib2022";

//const char* ssid = "CANALBOX-BA81";
//const char* password = "8955819532";

//---------Pinouts----------
#define SS_PIN 4
#define RST_PIN 5

//#define SS_PIN 8
//#define RST_PIN 0

//LCD confguration 
//const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 15;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//const int rs = , en = 0, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MFRC522 mfrc522(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 

byte nuidPICC[4];

String NUID_dec, NUID_hex, RKey;

//POST URL path or IP address with path
const char* serverName = "http://192.168.1.96:8181/api/logs/record_attendance"; 
//http://localhost:8081/api/logs/record_attendance/25 152 153 193

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
//  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);    

  for(byte i = 0 ; i < 6; i++){
    key.keyByte[i] = 0xFF;
  }

  RKey = String(key.keyByte[0], HEX) + " " 
        + String(key.keyByte[1], HEX) + " " 
        + String(key.keyByte[2], HEX) + " " 
        + String(key.keyByte[3], HEX) + " "
        + String(key.keyByte[4], HEX) + " " 
        + String(key.keyByte[5], HEX);
  RKey.toUpperCase();

  Serial.println("MFRC522 Key: ");
  Serial.print(RKey);

  //Wifi Connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
//  lcd.setCursor(0,0);
//  lcd.print("Swap Card..");
  
  MFRC522::StatusCode status;
  
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if(!mfrc522.PICC_ReadCardSerial()){
    return;  
  }
  Serial.println("Card Detected!");

  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details to serial

  //displaying the Non-Unique Identity in decimal
  NUID_dec = String(mfrc522.uid.uidByte[0])
           +String(mfrc522.uid.uidByte[1])
           +String(mfrc522.uid.uidByte[2])
           +String(mfrc522.uid.uidByte[3]);
  Serial.print("NUID Tag (DEC): "); 
  Serial.println(NUID_dec); 

  //displaying the Non-Unique Identity in dexi-decimal
  NUID_hex = String(mfrc522.uid.uidByte[0], HEX) + " "
           +String(mfrc522.uid.uidByte[1], HEX) + " "
           +String(mfrc522.uid.uidByte[2], HEX) + " " 
           +String(mfrc522.uid.uidByte[3], HEX);
  NUID_hex.toUpperCase();
  Serial.print("NUID Tag (HEX): "); 
  Serial.println(NUID_hex);
  
  delay(500);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  if ((millis() - lastTime) > timerDelay) {
    pushData();
    lastTime = millis();
  }

}

void pushData(){
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // Data to send with HTTP POST
      String httpRequestData = "uid=";  
      httpRequestData += NUID_dec;
      Serial.println(httpRequestData);     
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      String payload = http.getString();
        if( payload == "1"){
          Serial.println("successfull");
          Serial1.print("successfull");
        }else if( payload == "0"){
          Serial.println("not registered ");
          Serial1.print("not_found");
        }else if( payload == "2"){
            Serial.println("duplicate");
            Serial1.print("duplicate");
       }else{
        Serial.println("Unkown error");
        Serial1.print("Unkown_error");
        }
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      http.end();// Free resources
    }else {
      Serial.println("WiFi Disconnected");
    }
}
