#include <Bluepad32.h>
unsigned long ultimoMensaje = 0;
const unsigned long TIMEOUT_MS = 300
int vel_max = 255; 
int velocidadMotorA = 0;
int velocidadMotorB = 0;
#define PWMA 22
#define PWMB 2
#define INA1 5
#define INA2 4
#define INB1 19
#define INB2 21
#define STBY 18
GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
void onConnectedGamepad(GamepadPtr gp) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == nullptr) {
            myGamepads[i] = gp;
            break;
        }
    }
}
void onDisconnectedGamepad(GamepadPtr gp) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == gp) {
            myGamepads[i] = nullptr;
            break;
        }
    }
}
void moverMotor(int in1, int in2, int pwm, int velocidad) {
    velocidad = constrain(velocidad, -vel_max, vel_max);
    if (velocidad > 0) {
        analogWrite(pwm, velocidad);
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
    } else if (velocidad < 0) {
        analogWrite(pwm, -velocidad);
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
    } else {
        analogWrite(pwm, 0);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
    }
}
void moverRobot(int velA, int velB) {
    digitalWrite(STBY, (velA == 0 && velB == 0) ? LOW : HIGH);
    moverMotor(INA1, INA2, PWMA, velA);
    moverMotor(INB1, INB2, PWMB, velB);
}
void saludo() {
    moverRobot(255, 255);
    delay(100);
    for (int i = 0; i <= 3; i++) {
        moverRobot(255, -255);
        delay(100);
        moverRobot(-255, 255);
        delay(100);
    }
    moverRobot(-255, -255);
    delay(100);
}
void procesarEntradaMando(GamepadPtr gp) {
    ultimoMensaje = millis(); 

    if (gp->b()) {
        saludo();
    }

    int32_t gatilloR2 = gp->throttle(); 
    int32_t gatilloL2 = gp->brake();    
    if (gatilloR2 > 500) vel_max++;
    if (gatilloL2 > 500) vel_max--;
    vel_max = constrain(vel_max, 0, 255);

    int32_t avance = map(gp->axisY(), -512, 511, vel_max, -vel_max);
    int32_t giro = map(gp->axisRX(), -512, 511, -vel_max, vel_max);

    if (abs(gp->axisY()) < 45)  avance = 0;
    if (abs(gp->axisRX()) < 45) giro = 0;

    velocidadMotorA = avance + giro; 
    velocidadMotorB = avance - giro; 
}
void setup() {
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(INB1, OUTPUT);
    pinMode(INB2, OUTPUT);
    pinMode(INA1, OUTPUT);
    pinMode(INA2, OUTPUT);
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, LOW); 
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys();
}
void loop() {
    BP32.update();
    bool mandoActivo = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        GamepadPtr myGamepad = myGamepads[i];
        if (myGamepad && myGamepad->isConnected()) {
            procesarEntradaMando(myGamepad);
            mandoActivo = true;
        }
    }
    if (mandoActivo && (millis() - ultimoMensaje <= TIMEOUT_MS)) {
        moverRobot(velocidadMotorA, velocidadMotorB);
    } else {
        moverRobot(0, 0);
    }
    delay(10);
}