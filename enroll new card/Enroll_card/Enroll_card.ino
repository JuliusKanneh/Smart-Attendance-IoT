#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>


const char* ssid = "GEEKS-TEAM";
const char* password = "com.keypass@rw-lib2022";

//---------Pinouts----------
#define SS_PIN 4
#define RST_PIN 5

AsyncWebServer server(80);

MFRC522 mfrc522(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 

byte nuidPICC[4];

String NUID_dec, NUID_hex, RKey;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
//  lcd.begin(16, 2);
  Serial.begin(9600);
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

    server.begin();
}

void loop() {

  
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
  NUID_dec = String(mfrc522.uid.uidByte[0])+
           String(mfrc522.uid.uidByte[1]) +
           String(mfrc522.uid.uidByte[2]) +
           String(mfrc522.uid.uidByte[3]);
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
    
    server.on("*", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->url() == "/data") {
       AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"uid\":"+String(NUID_dec)+"}");
       response->addHeader("Access-Control-Allow-Origin","*");
       request->send(response);
       Serial.print("card id :"); Serial.println(NUID_dec);
    }
    else{
       request->send(SPIFFS, request->url(), String());
    }
  });
    lastTime = millis();
  }

delay(5000);
}
