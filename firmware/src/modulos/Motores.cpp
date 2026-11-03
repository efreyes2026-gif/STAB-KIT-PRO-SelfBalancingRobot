#include "Motores.h"
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"

int Motores::pwmActualA = 0;
int Motores::pwmActualB = 0;

void Motores::iniciar() {
    // Configuración de pines de dirección digital TB6612FNG
    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_BIN1, OUTPUT);
    pinMode(PIN_BIN2, OUTPUT);

    // Configuración de LEDs indicadores (Reglas 11 y 17)
    pinMode(PIN_LED_DIR_A, OUTPUT);
    pinMode(PIN_LED_DIR_B, OUTPUT);
    pinMode(PIN_LED_PWM_A, OUTPUT);
    pinMode(PIN_LED_PWM_B, OUTPUT);

    // Configuración de canales PWM en el ESP32 (LEDC)
    ledcSetup(LAB_PWM_CANAL_A, LAB_PWM_FREQ, LAB_PWM_RES_BITS);
    ledcAttachPin(PIN_PWMA, LAB_PWM_CANAL_A);

    ledcSetup(LAB_PWM_CANAL_B, LAB_PWM_FREQ, LAB_PWM_RES_BITS);
    ledcAttachPin(PIN_PWMB, LAB_PWM_CANAL_B);

    // Estado inicial en reposo
    frenar();
}

void Motores::frenar() {
    // Freno activo en TB6612FNG (0-0)
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, LOW);

    ledcWrite(LAB_PWM_CANAL_A, 0);
    ledcWrite(LAB_PWM_CANAL_B, 0);

    digitalWrite(PIN_LED_DIR_A, LOW);
    digitalWrite(PIN_LED_DIR_B, LOW);
    digitalWrite(PIN_LED_PWM_A, LOW);
    digitalWrite(PIN_LED_PWM_B, LOW);

    pwmActualA = 0;
    pwmActualB = 0;
}

void Motores::setVelocidadA(int pwm) {
    pwm = constrain(pwm, -255, 255);
    pwmActualA = pwm;
    int absPwm = abs(pwm);

    if (pwm > 0) {
        // Giro adelante: AIN1 = 1, AIN2 = 0
        digitalWrite(PIN_AIN1, HIGH);
        digitalWrite(PIN_AIN2, LOW);
        digitalWrite(PIN_LED_DIR_A, HIGH);
    } else if (pwm < 0) {
        // Giro atrás: AIN1 = 0, AIN2 = 1
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, HIGH);
        digitalWrite(PIN_LED_DIR_A, LOW);
    } else {
        // Freno si el valor es 0
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, LOW);
        digitalWrite(PIN_LED_DIR_A, LOW);
    }

    ledcWrite(LAB_PWM_CANAL_A, absPwm);
    analogWrite(PIN_LED_PWM_A, absPwm);
}

void Motores::setVelocidadB(int pwm) {
    pwm = constrain(pwm, -255, 255);
    pwmActualB = pwm;
    int absPwm = abs(pwm);

    if (pwm > 0) {
        // Giro adelante: BIN1 = 1, BIN2 = 0
        digitalWrite(PIN_BIN1, HIGH);
        digitalWrite(PIN_BIN2, LOW);
        digitalWrite(PIN_LED_DIR_B, HIGH);
    } else if (pwm < 0) {
        // Giro atrás: BIN1 = 0, BIN2 = 1
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, HIGH);
        digitalWrite(PIN_LED_DIR_B, LOW);
    } else {
        // Freno si el valor es 0
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, LOW);
        digitalWrite(PIN_LED_DIR_B, LOW);
    }

    ledcWrite(LAB_PWM_CANAL_B, absPwm);
    analogWrite(PIN_LED_PWM_B, absPwm);
}

void Motores::setVelocidades(int pwmA, int pwmB) {
    setVelocidadA(pwmA);
    setVelocidadB(pwmB);
}

int Motores::obtenerPwmA() {
    return pwmActualA;
}

int Motores::obtenerPwmB() {
    return pwmActualB;
}