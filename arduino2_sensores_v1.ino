// ARDUINO SENSORES

#include <Wire.h>
#include <DHT.h> 
#define DHTPIN 2 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <BH1750.h> 
BH1750 luzSensor(0x23);

bool actFlag=false;
int flag=0;

//PID
#define Umax 36  //Temp. mín. diurna
#define Umin 10  //Temp. máx. diurna
#define T 0.09 // sampling time

//long valor;

int in1 = 9;  // Pin que controla el sentido de giro Motor A
int in2 = 8;  // Pin que controla el sentido de giro Motor A
int EnA = 10; // Pin que controla la velocidad del  Motor A

double valorDHT22,valorDHT22_prec;
int pwmVent;

double setpoint = 20; //Checar decimales
double error;
double P, I, D, U;
double I_prec=0, U_prec=0, D_prec=0; 
boolean Saturado = false;

double Kp = 1; 
double Ki = 2; 
double Kd = 0;
//

void setup(){
  Serial.begin(19200);

  dht.begin();
  Wire.begin();

  if (luzSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    //Serial.println("BH1750 iniciado correctamente.");
  } else {
    //Serial.println("[BH1750] Error al iniciar el sensor. Revisa las conexiones o la alimentación.");
  }

  //
  pinMode(in1, OUTPUT);    // Configura  los pines como salida
  pinMode(in2, OUTPUT);
  pinMode(EnA, OUTPUT);
  //
  
  //Serial.println("LISTO");
}

void loop(){
  
  if (Serial.available() > 0) {        //wait for data available
    //Serial.print("Mensaje: ");
    String teststr = Serial.readString();   //read until timeout
    teststr.trim();                         // remove any \r \n whitespace at the end of the String
    flag=teststr.toInt();
    //Serial.println(actFlag);
    if(flag==1){
      actFlag=true;
    }else{
      actFlag=false;
    }
  }
   
  if(actFlag){
    
    Serial.println(tempDHT22());
    delay(5000);
    Serial.println(humDHT22());
    delay(5000);
    Serial.println(aireMQ135());
    delay(5000);
    Serial.println(luzBH1750());
    delay(5000);
    Serial.println(leerHumedadTierra());
    delay(5000);
    actFlag=false;
  }

  //////////////
  //entrada DHT22
  valorDHT22 = dht.readTemperature();
  //valorDHT22=map(valorDHT22, 0, 1023, 0, 80);
  //valorDHT22=valorDHT22/2;
  //Serial.print("Temp: ");
  //Serial.println(valorDHT22);
  delay(3);

  error = valorDHT22 - setpoint;   //grados

  P = Kp*error;

  if ( ! Saturado )
  
  I = I_prec + T*Ki*error;
  
  D = (Kd/T)*(valorDHT22 - valorDHT22_prec);
  
  U = P + I + D; 
  
  if ( U < Umin)  {
    U=Umin; 
    Saturado = true;
  }
  
  else if ( U > Umax) {
    U=Umax; 
    Saturado = true;
  }
  
  else  Saturado = false; 


  ///
  pwmVent=map(U, Umin, Umax, 50, 255);
  //Serial.print("PWM: ");
  //Serial.println(pwmVent);
  analogWrite(EnA,pwmVent);
  digitalWrite(in1, LOW);  // GIRO DERECHA
  digitalWrite(in2, HIGH);
  
  
  
  delay(24);
  /*
  Serial.print("angulo=");
  Serial.println(analogRead(0));
  Serial.println(velocidadMotor);
  */
  ///

  I_prec = I;
  valorDHT22_prec = valorDHT22;
  D_prec = D;
  ///////////////////
}


float tempDHT22(){
  float temperatura = dht.readTemperature();

  if (isnan(temperatura)) {
    //Serial.println("Error al leer del sensor DHT!");
  } else {
    //Serial.print("Temperatura: ");
    //Serial.print(temperatura);
    //Serial.println(" *C");
  }
  return temperatura;
}

float humDHT22(){
  float humedad = dht.readHumidity();

  if (isnan(humedad)) {
    //Serial.println("Error al leer del sensor DHT!");
  } else {
    //Serial.print("Humedad: ");
    //Serial.print(humedad);
    //Serial.print(" %\t");
  }
  return humedad;
}

int aireMQ135(){
  int valorMQ135 = analogRead(A0);
  //Serial.print("MQ-135 Valor: ");
  //Serial.println(valorMQ135);
  return valorMQ135;
}

float luzBH1750(){
  float lux = luzSensor.readLightLevel();
  if (lux == -2.00) {
    //Serial.println("[BH1750] Error al leer la luminosidad!");
  } else {
    //Serial.print("Luminosidad: ");
    //Serial.print(lux);
    //Serial.println(" lx");
  }
  return lux;
}

int leerHumedadTierra() {
  int valorHumedadTierra = analogRead(A1);
  //Serial.print("Humedad de Tierra Valor: ");
  //Serial.println(valorHumedadTierra);
  return valorHumedadTierra;
}
