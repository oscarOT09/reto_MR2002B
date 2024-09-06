// ESP 32 | Actuadores
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
String zero="0";

int agua=-1, foco=-1, ventana=-1, ventilador=-1,
    aguaC=-1, focoC=-1, ventanaC=-1,
    prevAgua=-1, prevFoco=-1, prevVentana=-1, prevVentilador=-1,
    prevAguaC=-1, prevFocoC=-1, prevVentanaC=-1;
bool aguaB=false, focoB=false, ventanaB=false, ventiladorB=false;

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
  if(Firebase.ready() && signupOK && ((millis() - prevMillis > 5000) || prevMillis == 0)){
    //Control automático
    if(Firebase.RTDB.getInt(&fbdo,"controladores/bombas")){
      aguaC = fbdo.intData();
      Serial.print("/aguaC: ");
      Serial.println(aguaC);
      
      if(aguaC != prevAguaC){
        if(aguaC==1){
          Serial.println("Bomba On");
          Serial2.print(1);
          if(Firebase.RTDB.setString(&fbdo, "Actuadores/Agua", zero)){
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
          }
          else{
            Serial.println("FAILED");
          }
        }
        else{
          Serial.println("Bomba Off");
          Serial2.print(2);
        }
        
      }
    }
    delay(500);
    
    if(Firebase.RTDB.getInt(&fbdo,"controladores/foco")){
      focoC = fbdo.intData();
      Serial.print("/focoC: ");
      Serial.println(focoC);
      
      if(focoC != prevFocoC){
        if(focoC==1){
          Serial.println("Foco On");
          Serial2.print(3);
        }else{
          Serial.println("Foco Off");
          Serial2.print(4);
        }
      }
    }
    delay(500);
    
    if(Firebase.RTDB.getInt(&fbdo,"controladores/ventana")){
      ventanaC = fbdo.intData();
      Serial.print("/ventanaC: ");
      Serial.println(ventanaC);
      
      if(ventanaC != prevVentanaC){
        if(ventanaC==1){
          Serial.println("Servos On");
          Serial2.print(5);
        }else{
          Serial.println("Servos Off");
          Serial2.print(6);
        }
      }
    }
    delay(500);
    
    Serial.println();
    
    //Control manual
    if(Firebase.RTDB.getInt(&fbdo,"Actuadores/Agua")){
      agua = (fbdo.stringData()).toInt();
      Serial.print("/Agua: ");
      Serial.println(agua);
      
      if(agua != prevAgua){
        if(agua==1){
          Serial.println("Bomba On");
          Serial2.print(1);
        }else{
          Serial.println("Bomba Off");
          Serial2.print(2);
        }
      }
    }
    delay(500);
    
    if(Firebase.RTDB.getString(&fbdo,"Actuadores/Foco")){
      foco = (fbdo.stringData()).toInt();
      Serial.print("/Foco: ");
      Serial.println(foco);
      
      if(foco != prevFoco){
        if(foco==1){
          Serial.println("Foco On");
          Serial2.print(3);
        }else{
          Serial.println("Foco Off");
          Serial2.print(4);
        }
      }
    }
    delay(500);

    if(Firebase.RTDB.getString(&fbdo,"Actuadores/Ventana")){
      ventana = (fbdo.stringData()).toInt();
      Serial.print("/Ventana: ");
      Serial.println(ventana);
      
      if(ventana != prevVentana){
        if(ventana==1){
          Serial.println("Servos On");
          Serial2.print(5);
        }else{
          Serial.println("Servos Off");
          Serial2.print(6);
        }
      }
    }
    delay(500);
    
    if(Firebase.RTDB.getString(&fbdo,"Actuadores/Ventilador")){
      ventilador = (fbdo.stringData()).toInt();
      Serial.print("/Ventilador: ");
      Serial.println(ventilador);
      
      if(ventilador != prevVentilador){
        if(ventilador==1){
          Serial.println("Ventilador On ");
          Serial2.print(7);
        }else{
          Serial.println("Ventilador Off");
          Serial2.print(8);
        }
      }
    } 
    delay(500);
    
    prevAguaC = aguaC;
    prevFocoC = focoC;
    prevVentanaC = ventanaC;

    prevAgua = agua;
    prevFoco = foco;
    prevVentana = ventana;
    prevVentilador = ventilador;
    
    Serial.println();
    prevMillis = millis();
  }
}
