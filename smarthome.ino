#include<Servo.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define SERVER "16.16.16.247"
#define USER "admin"
#define PASS "123"

#define PIR D8
#define SERVO D1
#define LED D0
Servo s;
int hay;
String jwt;
int sesion = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PIR,INPUT);
  s.attach(SERVO);
  s.write(0);

  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Wifi", "123456789");
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
      if(sesion) post();
      else login();
   }
   
  delay(10000);
}

void login(){
  WiFiClient client;
  HTTPClient http;
  if(http.begin(client, "http://" SERVER "/smarthome/login.php?user="USER"&pass="PASS)){
    int httpCode = http.GET();
    if(httpCode > 0){
      if(httpCode == HTTP_CODE_OK){
        String json = http.getString();
        Serial.println(json);
        int i,j;
        String r = json.substring(10,11);
        Serial.println(r);
        if(r == "y"){
          int inicio = json.indexOf("token");
          int fin = json.indexOf("}");
          jwt = json.substring(inicio+8,fin-1);
          sesion = 1;
          Serial.println(jwt);
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

void post(){
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://" SERVER "/smarthome/registro.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", jwt);
  int httpCode = http.POST("sensor=P&valor="+String(hay));
  if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println(payload);
      }else{
        Serial.println("No se registro el sensor");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}