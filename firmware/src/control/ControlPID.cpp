#include "ControlPID.h"
#include "ConfigTuning.h"

// Inicialización de variables estáticas con valores predeterminados
float ControlPID::kp = TUNING_PID_KP;
float ControlPID::ki = TUNING_PID_KI;
float ControlPID::kd = TUNING_PID_KD;
float ControlPID::errorPrevio = 0.0f;
float ControlPID::terminoIntegral = 0.0f;

void ControlPID::iniciar() {
    kp = TUNING_PID_KP;
    ki = TUNING_PID_KI;
    kd = TUNING_PID_KD;
    resetear();
}

void ControlPID::resetear() {
    errorPrevio = 0.0f;
    terminoIntegral = 0.0f;
}

void ControlPID::configurarGanancias(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
}

int ControlPID::calcular(float anguloActual, float dt) {
    // 1. Evitar división por cero en dt
    if (dt <= 0.0001f) {
        dt = 0.01f; // 10 ms por defecto
    }

    // 2. Setpoint efectivo con calibración de centro de masa
    float setpoint = TUNING_SETPOINT_ANGULO + TUNING_OFFSET_CENTRO_MASA;

    // 3. Cálculo del error (Consigna - Medición)
    float error = setpoint - anguloActual;

    // 4. Término Proporcional
    float P = kp * error;

    // 5. Término Integral con protección Anti-Windup
    terminoIntegral += error * dt;
    terminoIntegral = constrain(terminoIntegral, -TUNING_PID_INTEGRAL_MAX, TUNING_PID_INTEGRAL_MAX);
    float I = ki * terminoIntegral;

    // 6. Término Derivativo (Derivada del error)
    float D = kd * ((error - errorPrevio) / dt);
    errorPrevio = error;

    // 7. Salida en lazo cerrado
    float salidaControl = P + I + D;

    // 8. Compensación de zona muerta (Deadband) para romper fricción estática
    int pwmSalida = 0;
    if (salidaControl > 0.5f) {
        pwmSalida = (int)salidaControl + TUNING_DEADBAND_PWM;
    } else if (salidaControl < -0.5f) {
        pwmSalida = (int)salidaControl - TUNING_DEADBAND_PWM;
    } else {
        pwmSalida = 0;
    }

    // 9. Saturación al rango admisible de 8 bits del TB6612FNG (-255 a +255)
    return constrain(pwmSalida, -255, 255);
}