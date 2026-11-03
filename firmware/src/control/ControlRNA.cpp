#include "ControlRNA.h"
#include "ConfigTuning.h"
#include <math.h>

float ControlRNA::errorPrevio = 0.0f;

// =========================================================================
// MATRICES DE PESOS Y SESGOS PREDETERMINADOS (ARQUITECTURA 2 -> 5 -> 1)
// Red entrenada/sintetizada para equilibrio de péndulo invertido
// =========================================================================

// Pesos Capa Oculta W1 [5 neuronas x 2 entradas (Error, Derivada)]
static const float W1[5][2] = {
    {  1.450f,  0.350f },   // Neurona 1: Reacción inclinación positiva
    { -1.450f, -0.350f },   // Neurona 2: Reacción inclinación negativa
    {  2.850f,  0.820f },   // Neurona 3: Caídas rápidas / choques fuertes
    { -2.850f, -0.820f },   // Neurona 4: Recuperación de emergencia
    {  0.600f,  0.500f }    // Neurona 5: Estabilización fina alrededor de 0°
};

// Sesgos Capa Oculta b1 [5 neuronas]
static const float b1[5] = {
     0.100f,
    -0.100f,
     0.050f,
    -0.050f,
     0.000f
};

// Pesos Capa de Salida W2 [1 salida x 5 neuronas ocultas]
static const float W2[5] = {
    0.650f,
   -0.650f,
    0.920f,
   -0.920f,
    0.450f
};

// Sesgo Capa de Salida b2 [1 salida]
static const float b2 = 0.000f;

void ControlRNA::iniciar() {
    resetear();
}

void ControlRNA::resetear() {
    errorPrevio = 0.0f;
}

float ControlRNA::funcionTanh(float x) {
    return tanhf(x);
}

int ControlRNA::calcular(float anguloActual, float dt) {
    if (dt <= 0.0001f) dt = 0.01f;

    // 1. Cálculo de Error y Derivada
    float setpoint = TUNING_SETPOINT_ANGULO + TUNING_OFFSET_CENTRO_MASA;
    float e = setpoint - anguloActual;
    float de = (e - errorPrevio) / dt;
    errorPrevio = e;

    // 2. Normalización de entradas al rango [-1.0, 1.0]
    // Consideramos rangos físicos máximos normales: +-30 grados y +-150 deg/s
    float x1 = constrain(e / 30.0f, -1.0f, 1.0f);
    float x2 = constrain(de / 150.0f, -1.0f, 1.0f);

    // 3. Inferencia Capa Oculta (5 Neuronas con activación tanh)
    float a[5];
    for (int j = 0; j < 5; j++) {
        float z = (W1[j][0] * x1) + (W1[j][1] * x2) + b1[j];
        a[j] = funcionTanh(z);
    }

    // 4. Inferencia Capa de Salida (Combinación lineal)
    float u_norm = b2;
    for (int j = 0; j < 5; j++) {
        u_norm += W2[j] * a[j];
    }
    u_norm = constrain(u_norm, -1.0f, 1.0f);

    // 5. Desescalado a rango PWM de 8 bits (-255 a +255)
    float esfuerzoPWM = u_norm * 255.0f;

    // 6. Inyección de Zona Muerta (Deadband)
    int pwmSalida = 0;
    if (esfuerzoPWM > 1.0f) {
        pwmSalida = (int)esfuerzoPWM + TUNING_DEADBAND_PWM;
    } else if (esfuerzoPWM < -1.0f) {
        pwmSalida = (int)esfuerzoPWM - TUNING_DEADBAND_PWM;
    } else {
        pwmSalida = 0;
    }

    return constrain(pwmSalida, -255, 255);
}