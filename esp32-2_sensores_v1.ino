// SENSORES ESP

#include "Arduino.h"
#include <WiFi.h>

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include "addons/RTDBHelper.h"

#define RXD2 16
#define TXD2 17

//API del proyecto de Firebase
#define API_KEY "AIzaSyBssIK1e-o74NFAaUmKYW1hRndKJF-Vnsg"
//URL de base de datos en tiempo real
#define DATABASE_URL "https://invernadero-7b818-default-rtdb.firebaseio.com"

//Datos de objeto Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//Datos de acceso WiFi
char ssid[] = "VICTUS 8582";
char pass[] = "7177L5-j";

//Variables generales
bool signupOK = false;
unsigned long prevMillis = 0;

int cont=0,intValue=0, connectionDelay = 3;

bool flag=false;

void setup() {
  Serial.begin(19200);  //Inicio del puerto Serial
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);

  //Impresión de SSID a conectar
  Serial.print("Connecting to ");
  Serial.println(ssid);

  Serial.println("Connecting to Wi-Fi...");
  // Loop para lograr una conexión exitosa
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000);
    Serial.println( WiFi.status() ); 
  }
  Serial.println( "Connected to Wi-Fi." );  

  //Asignación de credenciales de la base de datos en tiempo real
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Connected to Firebase");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("System ready");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Connected to Firebase");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  
  if(Firebase.ready() && signupOK){ 
      Serial2.println(1);
      Serial.println("Sensado");
      flag=true;
  }

  while(flag){
    while (Serial2.available() == 0);
    String teststr = Serial2.readString();
    teststr.trim();
    Serial.println(teststr);
    intValue = teststr.toInt();
    Serial.print("Converted to int: ");
    Serial.println(intValue);

    if(cont==0){
      if(Firebase.RTDB.setInt(&fbdo, "Sensores/Temperatura", intValue)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }
      cont+=1;
    }else if(cont==1){
      if(Firebase.RTDB.setInt(&fbdo, "Sensores/Humedad", intValue)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }
      cont+=1;
    }else if(cont==2){
      if(Firebase.RTDB.setInt(&fbdo, "Sensores/HumedadTierra", intValue)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }

      if(intValue>400){
        if(Firebase.RTDB.setInt(&fbdo, "controladores/bombas", 1)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }
      }

      cont+=1;
    }else if(cont==3){
      if(Firebase.RTDB.setInt(&fbdo, "Sensores/Luz", intValue)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }

      if(intValue<200){
        if(Firebase.RTDB.setInt(&fbdo, "controladores/foco", 1)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
        }else{
          Serial.println("FAILED");
        }
      }else{
        if(Firebase.RTDB.setInt(&fbdo, "controladores/foco", 0)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
        }else{
          Serial.println("FAILED");
        }
      }

      cont+=1;
    }else if(cont==4){
      if(Firebase.RTDB.setInt(&fbdo, "Sensores/Aire", intValue)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
      }else{
        Serial.println("FAILED");
      }
      cont=0;
      flag=false;
      Serial2.println(0);
      Serial.println("Fin del sensado");
    }
  }
  delay(300000);

  
}

void connectWifi()
{
  Serial.println( "Connecting to Wi-Fi..." );
  // Loop until WiFi connection is successful
    while ( WiFi.status() != WL_CONNECTED ) {
    //WiFi.begin( ssid, pass );
    WiFi.begin(ssid);
    delay( connectionDelay*1000 );
    Serial.println( WiFi.status() ); 
  }
  Serial.println( "Connected to Wi-Fi." );
}