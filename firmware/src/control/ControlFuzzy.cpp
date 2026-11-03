#include "ControlFuzzy.h"
#include "ConfigTuning.h"

float ControlFuzzy::errorPrevio = 0.0f;

void ControlFuzzy::iniciar() {
    resetear();
}

void ControlFuzzy::resetear() {
    errorPrevio = 0.0f;
}

// Funciones estándar de membresía
float ControlFuzzy::pertenenciaTrapecio(float x, float a, float b, float c, float d) {
    if (x <= a || x >= d) return 0.0f;
    if (x >= b && x <= c) return 1.0f;
    if (x > a && x < b) return (x - a) / (b - a);
    if (x > c && x < d) return (d - x) / (d - c);
    return 0.0f;
}

float ControlFuzzy::pertenenciaTriangulo(float x, float a, float b, float c) {
    if (x <= a || x >= c) return 0.0f;
    if (x == b) return 1.0f;
    if (x > a && x < b) return (x - a) / (b - a);
    if (x > b && x < c) return (c - x) / (c - b);
    return 0.0f;
}

int ControlFuzzy::calcular(float anguloActual, float dt) {
    if (dt <= 0.0001f) dt = 0.01f;

    /*// 1. Cálculo de Error y Derivada escalados con ConfigTuning.h
    float setpoint = TUNING_SETPOINT_ANGULO + TUNING_OFFSET_CENTRO_MASA;
    float e = (setpoint - anguloActual) * TUNING_FUZZY_GE;
    float de = ((e - errorPrevio) / dt) * TUNING_FUZZY_GDE;
    errorPrevio = e;*/

    // 1. Cálculo de Error y Derivada desacoplados
    float setpoint = TUNING_SETPOINT_ANGULO + TUNING_OFFSET_CENTRO_MASA;
    float e_fisico = setpoint - anguloActual;
    float de_fisico = (e_fisico - errorPrevio) / dt;
    errorPrevio = e_fisico;

    // Aplicación independiente de factores de escala
    float e  = e_fisico  * TUNING_FUZZY_GE;
    float de = de_fisico * TUNING_FUZZY_GDE;

    // 2. FUSIFICACIÓN: 5 conjuntos para Error (e) en [-30°, +30°]
    // NG: Negativo Grande, NP: Negativo Pequeño, ZE: Cero, PP: Positivo Pequeño, PG: Positivo Grande
    float mu_e[5];
    mu_e[0] = pertenenciaTrapecio(e, -100.0f, -100.0f, -20.0f, -8.0f); // NG
    mu_e[1] = pertenenciaTriangulo(e, -15.0f, -7.0f, 0.0f);            // NP
    mu_e[2] = pertenenciaTriangulo(e, -5.0f, 0.0f, 5.0f);              // ZE
    mu_e[3] = pertenenciaTriangulo(e, 0.0f, 7.0f, 15.0f);              // PP
    mu_e[4] = pertenenciaTrapecio(e, 8.0f, 20.0f, 100.0f, 100.0f);    // PG

    // 3. FUSIFICACIÓN: 5 conjuntos para Derivada (de) en [-120°/s, +120°/s]
    float mu_de[5];
    mu_de[0] = pertenenciaTrapecio(de, -300.0f, -300.0f, -80.0f, -20.0f); // NG
    mu_de[1] = pertenenciaTriangulo(de, -60.0f, -25.0f, 0.0f);            // NP
    mu_de[2] = pertenenciaTriangulo(de, -20.0f, 0.0f, 20.0f);             // ZE
    mu_de[3] = pertenenciaTriangulo(de, 0.0f, 25.0f, 60.0f);              // PP
    mu_de[4] = pertenenciaTrapecio(de, 20.0f, 80.0f, 300.0f, 300.0f);     // PG

    // 4. MATRIZ DE REGLAS (5x5) - Salida asociada a cada combinación
    // Valores consecuentes de salida (Singletons de PWM):
    const float O_NG = -255.0f;
    const float O_NP = -140.0f;
    const float O_ZE = 0.0f;
    const float O_PP = 140.0f;
    const float O_PG = 255.0f;

    const float tablaReglas[5][5] = {
        /*          de: NG     NP    ZE    PP    PG  */
        /* e: NG */ { O_NG, O_NG, O_NG, O_NP, O_ZE },
        /* e: NP */ { O_NG, O_NG, O_NP, O_ZE, O_PP },
        /* e: ZE */ { O_NG, O_NP, O_ZE, O_PP, O_PG },
        /* e: PP */ { O_NP, O_ZE, O_PP, O_PG, O_PG },
        /* e: PG */ { O_ZE, O_PP, O_PG, O_PG, O_PG }
    };

    // 5. INFERENCIA Y DEFUSIFICACIÓN (Centroide / Promedio Ponderado)
    float sumaPesos = 0.0f;
    float sumaPonderada = 0.0f;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Operador AND difuso: Mínimo entre pertenencias
            float activacion = fmin(mu_e[i], mu_de[j]);
            if (activacion > 0.0f) {
                sumaPesos += activacion;
                sumaPonderada += activacion * tablaReglas[i][j];
            }
        }
    }

    float salidaFuzzy = 0.0f;
    if (sumaPesos > 0.0001f) {
        salidaFuzzy = (sumaPonderada / sumaPesos) * TUNING_FUZZY_GU;
    }

    // 6. Inyección de Zona Muerta (Deadband)
    int pwmSalida = 0;
    if (salidaFuzzy > 1.0f) {
        pwmSalida = (int)salidaFuzzy + TUNING_DEADBAND_PWM;
    } else if (salidaFuzzy < -1.0f) {
        pwmSalida = (int)salidaFuzzy - TUNING_DEADBAND_PWM;
    } else {
        pwmSalida = 0;
    }

    return constrain(pwmSalida, -255, 255);
}