#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "CANALBOX-BA81";
const char* password = "8955819532";

//const char* ssid = "CANALBOX-DBD6";
//const char* password = "1316523275";

//---------Pinouts----------
#define SS_PIN 4
#define RST_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 

byte nuidPICC[4];

String NUID_dec, RKey;

//Servo servo;

//int counter;
String firstname1;

//POST URL path or IP address with path
const char* serverName = "http://192.168.1.96:8081/api/push";

//GET URL path or IP address with path
String getServerName = "http://192.168.1.96:8081/data";

//GET URL path or IP address with path
String serverName0 = "http://192.168.1.96:8081/push";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

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

  //function to make initial push 
//    initialPush();
}

void loop() {

  //some variables needed
  byte block;
  byte len;
  MFRC522::StatusCode status;
  
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if(!mfrc522.PICC_ReadCardSerial()){
    return;  
  }
  Serial.println("Card Detected!");

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details to serial

  byte buffer1[18];
  char firstname[18];
  block = 4;
  len = 18;

  //------------------------------ GET FIRST NAME STARTS ----------------------------
  Serial.print("First Name: ");
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if(status != MFRC522::STATUS_OK){
    Serial.print("Authentication Failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if(status != MFRC522::STATUS_OK){
    Serial.print("Reading Failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Print the name
  for(uint8_t i = 0; i < 16; i++){
    if(buffer1[i] != 32){
      Serial.write(buffer1[i]);
      firstname[i] = buffer1[i];
    }
  }
  
  firstname[0] = ' ';
  firstname1 = String(firstname);
//  firstname[13] = '\0'; //null terminator;
  
  Serial.println(" ");
  Serial.print("First Name: " );
//  Serial.println(firstname);
  Serial.print(firstname1);
  
  //------------------------------ GET FIRST NAME ENDS ----------------------------

  Serial.println(F("\n**End of Reading**\n"));
  delay(1000);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  pushData();

}

void initialPush(){
  //Check WiFi connection status
//    if(WiFi.status()== WL_CONNECTED){
//      WiFiClient client0;
//      HTTPClient http0;
//      
//      // Your Domain name with URL path or IP address with path
//      http0.begin(client0, serverName0);
//
//      // Specify content-type header
//      http0.addHeader("Content-Type", "application/x-www-form-urlencoded");
//      
//      // Data to send with HTTP POST
//      String httpRequestData = "temp=";  
//      httpRequestData += t; 
//      httpRequestData += "&hum=";   
//      httpRequestData += h;
//      Serial.println(httpRequestData);     
//      // Send HTTP POST request
//      int httpResponseCode = http0.POST(httpRequestData);
//     
//      Serial.print("HTTP Response code: ");
//      Serial.println(httpResponseCode);
//        
//      // Free resources
//      http0.end();
//    }else {
//      Serial.println("WiFi Disconnected");
//    }
}


void pushData(){
//  Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // Data to send with HTTP POST
      String httpRequestData = "name=";  
      httpRequestData += firstname1; 
//      httpRequestData += "&hum=";   
//      httpRequestData += h;
      Serial.println(httpRequestData);     
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      http.end();// Free resources
    }else {
      Serial.println("WiFi Disconnected");
    }
}


void getData(){
//  if ((millis() - lastTime) > timerDelay) {
//    //Check WiFi connection status
//    if(WiFi.status()== WL_CONNECTED){
//      WiFiClient client;
//      HTTPClient http;
//
//      String serverPath = getServerName + "/1";
//      
//      // Domain name with URL path or IP address with path
//      http.begin(client, serverPath.c_str());
//      http.addHeader("Content-Type", "application/x-www.form-urlencoded");
//      
//      // Send HTTP GET request
//      int httpResponseCode = http.GET();
//      
//      if (httpResponseCode>0) {
//        Serial.print("HTTP Response code: ");
//        Serial.println(httpResponseCode);
//        String payload = http.getString();
//        Serial.println(payload);
//
//        //calling method to extract temperature and humidity from the payload
//        extractPayload(payload);
//      }
//      else {
//        Serial.print("Error code: ");
//        Serial.println(httpResponseCode);
//      }
//      // Free resources
//      http.end();
//    }
//    else {
//      Serial.println("WiFi Disconnected");
//    }
//    lastTime = millis();
//  }
}


void extractPayload(String payload){
//  String temp_value;
//  String hum_value;
//
//  //extracting the temperature value from the payload
//  char temp_d1 = payload[1];
//  char temp_d2 = payload[2];
//  char temp_d3 = payload[3];
//  char temp_d4 = payload[4];
//
//  temp_value += temp_d1;
//  temp_value += temp_d2;
//  temp_value += temp_d3;
//  temp_value += temp_d4;
//        
//  Serial.println("temp value: ");
//  Serial.println(temp_value);
//  temp_double = temp_value.toDouble();
//  Serial.print("Temperature Value in double: ");
//  Serial.println(temp_double);
//
//  //extracting the humidity value from the payload
//  char hum_d1 = payload[6];
//  char hum_d2 = payload[7];
//  char hum_d3 = payload[8];
//  char hum_d4 = payload[9];
//
//  hum_value += hum_d1;
//  hum_value += hum_d2;
//  hum_value += hum_d3;
//  hum_value += hum_d4;
//        
//  Serial.println("hum value: ");
//  Serial.println(hum_value);
//  hum_double = hum_value.toDouble();
//  Serial.print("Humidity Value in double: ");
//  Serial.println(hum_double);
}
