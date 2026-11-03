#ifndef MODULO_ENCODERS_H
#define MODULO_ENCODERS_H

#include <Arduino.h>

class Encoders {
public:
    // Configura los pines con pull-up y asocia las interrupciones hardware
    static void iniciar();

    // Obtener las cuentas acumuladas de pulsos
    static long obtenerPulsosIzq();
    static long obtenerPulsosDer();

    // Reiniciar los contadores a cero
    static void reiniciar();

    // Rutinas de servicio de interrupción (ISR en RAM para alta velocidad)
    static void IRAM_ATTR isrIzq();
    static void IRAM_ATTR isrDer();

private:
    static volatile long pulsosIzq;
    static volatile long pulsosDer;
};

#endif // MODULO_ENCODERS_H