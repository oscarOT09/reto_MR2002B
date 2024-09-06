//BOMBAS AGUA
#define pinRelay1 3  // Relé 1
#define pinRelay2 2  // Relé 2

// SERVOS
#include <Servo.h>
Servo servo1,servo2;

// FOCO
#define pinFoco 4
int testint=0,flujo=10000;

unsigned long prevMillis = 0; // Para el control de los servos ON/OFF
unsigned long servosOnMillis = 0; // Para controlar los 15 segundos entre ON y OFF
bool flag = false;

void setup() {
  Serial.begin(19200);
  
  // BOMBAS AGUA
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);

  // SERVOS
  servo1.attach(9);
  servo2.attach(10);
  
  servo1.write(0);
  servo2.write(180);

  // FOCO
  pinMode(pinFoco, OUTPUT);
  bombasAguaOFF();
  focoOFF();
}

void loop(){
  // Manejo del puerto Serial (puedes eliminar esta parte si no es necesaria)
  if (Serial.available() > 0) {
    Serial.print("ESP: ");
    String teststr = Serial.readString();
    teststr.trim();
    testint = teststr.toInt();
    Serial.println(testint);
  
    if(testint == 1){
      bombasAguaON(flujo);
    } else if(testint == 2){
      bombasAguaOFF();
    } else if(testint == 3){
      focoON();
    } else if(testint == 4){
      focoOFF();
    } else if(testint == 5){
      servosON();
    } else if(testint == 6){
      servosOFF();
    }
  }

  // Control para activar SERVOS ON cada 30 segundos
  if ((millis() - prevMillis) > 1800000) {
    //Serial.println("SERVOS ON");
    servosON(); // Activa los servos
    servosOnMillis = millis(); // Almacena el tiempo en que los servos se encendieron
    flag = true; // Habilita la siguiente parte del código para apagar los servos
    prevMillis = millis(); // Reinicia el tiempo para el próximo ciclo de 30 segundos
  }

  // Control para apagar SERVOS OFF después de 15 segundos de haberlos encendido
  if (flag && (millis() - servosOnMillis > 300000)) {
    //Serial.println("SERVOS OFF");
    servosOFF(); // Apaga los servos
    flag = false; // Desactiva la bandera hasta el próximo ciclo
  }
}

void bombasAguaON(int flujo){
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, LOW);
  delay(flujo);
  bombasAguaOFF();
  Serial.println("Bomba On");
}

void bombasAguaOFF(){
  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, HIGH);
  delay(1000);
  Serial.println("Bomba OFF");
}

void servosOFF(){
  servo1.write(0);
  servo2.write(180);
  Serial.println("Servos Off");
}

void servosON(){
  servo1.write(135);
  servo2.write(45);
  Serial.println("Servos On");
}

void focoON(){
  digitalWrite(pinFoco, LOW);
  delay(5000);
  Serial.println("Foco On");
}

void focoOFF(){
  digitalWrite(pinFoco, HIGH);
  Serial.println("Foco Off");
}
