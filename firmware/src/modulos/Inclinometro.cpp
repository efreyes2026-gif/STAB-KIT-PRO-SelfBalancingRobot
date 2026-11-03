#include "Inclinometro.h"
#include <Wire.h>
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"

// Inicialización de variables estáticas
float Inclinometro::anguloPitchMPU = 0.0f;
float Inclinometro::anguloPitchGY61 = 0.0f;
int   Inclinometro::rawGY61 = 0;
unsigned long Inclinometro::tiempoPrevio = 0;

void Inclinometro::iniciar() {
    // Despertar el MPU-6050 (registro de gestión de energía 0x6B)
    Wire.beginTransmission(LAB_MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    tiempoPrevio = millis();
}

void Inclinometro::actualizar() {
    // 1. Lectura y mapeo del sensor analógico GY-61 (Pin 34)
    rawGY61 = analogRead(PIN_GY61_X);

    if (rawGY61 <= LAB_GY61_ADC_CENTRO) {
        anguloPitchGY61 = map(rawGY61, LAB_GY61_ADC_MIN, LAB_GY61_ADC_CENTRO, -900, 0) / 10.0f;
    } else {
        anguloPitchGY61 = map(rawGY61, LAB_GY61_ADC_CENTRO, LAB_GY61_ADC_MAX, 0, 900) / 10.0f;
    }

    // 2. Lectura acelerómetro digital MPU-6050 (Registros 0x3B a 0x40)
    Wire.beginTransmission(LAB_MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom((uint16_t)LAB_MPU_ADDR, (uint8_t)6, true);

    if (Wire.available() >= 6) {
        int16_t ax = Wire.read() << 8 | Wire.read();
        int16_t ay = Wire.read() << 8 | Wire.read();
        int16_t az = Wire.read() << 8 | Wire.read();

        // Estimación de inclinación por acelerometría pura
        float angAcc = atan2((float)ay, (float)az) * 180.0f / PI;

        // Cálculo diferencial de tiempo
        unsigned long ahora = millis();
        float dt = (ahora - tiempoPrevio) / 1000.0f;
        tiempoPrevio = ahora;

        // Filtro complementario con constante fija de laboratorio
        anguloPitchMPU = LAB_MPU_ALPHA_FILTRO * (anguloPitchMPU) + (1.0f - LAB_MPU_ALPHA_FILTRO) * angAcc;
    }
}

float Inclinometro::obtenerPitchMPU() {
    return anguloPitchMPU;
}

float Inclinometro::obtenerPitchGY61() {
    return anguloPitchGY61;
}

int Inclinometro::obtenerRawGY61() {
    return rawGY61;
}