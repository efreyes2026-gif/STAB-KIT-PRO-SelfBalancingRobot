#include "Encoders.h"
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"

// Inicialización de contadores volátiles
volatile long Encoders::pulsosIzq = 0;
volatile long Encoders::pulsosDer = 0;

void IRAM_ATTR Encoders::isrIzq() {
    pulsosIzq++;
}

void IRAM_ATTR Encoders::isrDer() {
    pulsosDer++;
}

void Encoders::iniciar() {
    // Configurar pines de entrada con resistencia pull-up interna
    pinMode(PIN_ENC_IZQ, INPUT_PULLUP);
    pinMode(PIN_ENC_DER, INPUT_PULLUP);

    // Adjuntar interrupciones al flanco de bajada (FALLING)
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_IZQ), isrIzq, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC_DER), isrDer, FALLING);

    reiniciar();
}

long Encoders::obtenerPulsosIzq() {
    return pulsosIzq;
}

long Encoders::obtenerPulsosDer() {
    return pulsosDer;
}

void Encoders::reiniciar() {
    pulsosIzq = 0;
    pulsosDer = 0;
}