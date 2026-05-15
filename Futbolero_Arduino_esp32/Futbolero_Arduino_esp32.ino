 /**
 * @Owner Santiago Rodriguez Villarruel -- www.Github.com/Sstrv
 * @file futbolero_ARDUINO_ESP32.ino
 * @brief Sketch de Arduino para controlar dos motores usando entradas tipo boton vía comunicación serial.
 * 
 * Este programa lee varias entrada tipo boton (tipo 'A','B','C','D') desde una conexión serial
 * y controla dos motores en consecuencia. El control de los motores se logra utilizando señales PWM
 * y lógica de puente H para establecer la dirección y velocidad de los motores.
 * 
 * @detalles
 * - El formato de entrada esperado es una cadena en el formato de un unico caracter.
 * - El caracter entante determina la direccion y velocidad de los motores.
 * - Las velocidades de los motores se limitan a un rango de -255 a 255.
 * - Se utiliza la biblioteca SoftwareSerial para la comunicación serial si se usa arduino.
 * - Se utiliza la biblioteca BluetoothSerial para la comunicación serial si se usa ESP32.
 * - Se puede configurar la señal pwm si se usa ESP32 usando ledcSetup, ledcAttachPin y ledcWrite como 
 * alternativa a analogWrite.
 *
 * @hardware
 * - Arduino Nano o ESP32.
 * - Dos motores DC conectados a un controlador de motor tipo puente H.
 * - Puente H o Driver para motor.
 * - Modlulo bluetooth HC-05, HC-06 en caso de que se use Arduino Nano.
 */
#include <SoftwareSerial.h> // Arduino
//#include <BluetoothSerial.h> // ESP32
#define PWMA 5
#define PWMB 10
#define INA1 6
#define INA2 7
#define INB1 9
#define INB2 8

#define L_UP          'A'
#define L_DOWN        'B'
#define L_UPRIGHT     'C'
#define L_UPLEFT      'D'
#define L_DOWNRIGHT   'E'
#define L_DOWNLEFT    'F'
#define L_RIGHT       'G'
#define L_LEFT        'H'
#define VELMAX        'Q'

#define DEBUG false   
unsigned long ultimoMensaje = 0;
const unsigned long TIMEOUT_MS = 300;

//#define CANAL_A 0
//#define CANAL_B 1
int vel = 0 ;
int vel_max = 255;
char mensaje = ' ';
//BluetoothSerial BT; // ESP32 
SoftwareSerial BT(0, 1); // RX, TX Arduino
void moverMotor(int in1, int in2, int pwm/* int canal_pwm*/ ,int velocidad ){
    velocidad = constrain(velocidad, -vel_max, vel_max);
    if (velocidad > 0) {
        analogWrite(pwm, velocidad);//Arduino ESP32(Algunos)
        //ledcWrite(canalPWM, velocidad);//ESP32 (Algunos)
        digitalWrite(in1, HIGH);  
        digitalWrite(in2, LOW);   
    } else if ((velocidad < 0)){
        analogWrite(pwm, -velocidad);//Arduino ESP32(Algunos)
        //ledcWrite(canalPWM, -velocidad);//ESP32 (Algunos)
        digitalWrite(in1, LOW);   
        digitalWrite(in2, HIGH);  
    } else {
       analogWrite(pwm, 0);//Arduino ESP32(Algunos)
        //ledcWrite(canalPWM, 0);//ESP32 (Algunos)
        digitalWrite(in1, LOW);   
        digitalWrite(in2, LOW);  
    }
}
void setup() {
  //ledcSetup(CANAL_A, 5000, 8); alternativa en ESP32
  //ledcSetup(CANAL_B, 5000, 8); alternativa en ESP32

  //ledcAttachPin(PWMA, CANAL_A); alternativa en ESP32
  //ledcAttachPin(PWMB, CANAL_B); alternativa en ESP32

  BT.begin(9600); //Arduino
  //BT.begin("CarroDe-----"); //ESP32 Nombre del dispositivo Bluetooth
  if (DEBUG){
    Serial.begin(115200);
    Serial.println("El dispositivo Bluetooth está listo para emparejarse.");
  }
  pinMode(PWMA,OUTPUT);
  pinMode(PWMB,OUTPUT);
  pinMode(INB1,OUTPUT);
  pinMode(INB2,OUTPUT);
  pinMode(INA1,OUTPUT);
  pinMode(INA2,OUTPUT);
}
void loop() {
  if (BT.available()) {
    mensaje = BT.read();
    ultimoMensaje = millis();
    #if DEBUG
      Serial.println(mensaje);
    #endif
  }
  if (millis() - ultimoMensaje > TIMEOUT_MS) {
    mensaje = ' '; // fuerza stop
  }
  if (mensaje == VELMAX)
    vel = 255;
  else if (mensaje >= '0' && mensaje <= '9'){
    vel = (mensaje - '0' ) * 255/10 ;
  }
  //----------------------------------------
  // Adelante
  //----------------------------------------
  else if (mensaje == L_UP) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, vel);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, vel);
  }
  //----------------------------------------
  // Atrás
  //----------------------------------------
  else if (mensaje == L_DOWN) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, -vel);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, -vel);
  }
  //----------------------------------------
  // Derecha
  //----------------------------------------
  else if (mensaje == L_RIGHT) {
    moverMotor(INA1, INA2, PWMA /*CANAL_A*/, vel);
    moverMotor(INB1, INB2, PWMB /*CANAL_B*/, -vel);
  }
  //----------------------------------------
  // Izquierda
  //----------------------------------------
  else if (mensaje == L_LEFT) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, -vel);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, vel);
  }
  //----------------------------------------
  // Diagonal adelante derecha
  //----------------------------------------
  else if (mensaje == L_UPRIGHT) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, vel);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, vel / 2);
  }
  //----------------------------------------
  // Diagonal adelante izquierda
  //----------------------------------------
  else if (mensaje == L_UPLEFT) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, vel / 2);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, vel);
  }
  //----------------------------------------
  // Diagonal atrás derecha
  //----------------------------------------
  else if (mensaje == L_DOWNRIGHT) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, -vel);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, -vel / 2);
  }
  //----------------------------------------
  // Diagonal atrás izquierda
  //----------------------------------------
  else if (mensaje == L_DOWNLEFT) {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, -vel / 2);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, -vel);
  }
  //----------------------------------------
  // Stop
  //----------------------------------------
  else {
    moverMotor(INA1, INA2, PWMA/*CANAL_A*/, 0);
    moverMotor(INB1, INB2, PWMB/*CANAL_B*/, 0);
  }
}