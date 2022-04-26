#include<Servo.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define PIR D8
#define SERVO D1
#define LED D0
Servo s;
int hay;
void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PIR,INPUT);
  s.attach(SERVO);
  s.write(0);

  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("IOT", "12345678");
}

void loop()
{

  hay = digitalRead(PIR);
  Serial.println(hay);
  if( hay == HIGH ){
    digitalWrite(LED,HIGH);
    s.write(180);
    delay(1000);
  }else{
    digitalWrite(LED,LOW);
    s.write(0);
    delay(1000);
  }

   if ((WiFiMulti.run() == WL_CONNECTED)) {
      Serial.println("Conexion exitosa");
      
   }
   
  delay(1000);
}

void login(){
  WiFiClient client;
  HTTPClient http;
  if(http.begin(client, "http://localhost/smarthome/login.php?user=admin&pass=123")){
    int httpCode = http.GET();
    if(httpCode > 0){
      if(httpCode == HTTP_CODE_OK){
        String json = http.getString();
        Serial.println(json);
        int i,j;
        String r = json.substring(10,1);
        Serial.println(r);
        if(r == "y"){
          Serial.println("Login exitoso");
        }else{
          Serial.println("Login fallido");
        }
      }
    }else{
      Serial.println("Error GET");
    }
  }
}
