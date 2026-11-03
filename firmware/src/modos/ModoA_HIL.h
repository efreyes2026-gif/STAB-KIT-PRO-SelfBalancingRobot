#ifndef MODO_A_HIL_H
#define MODO_A_HIL_H

#include <Arduino.h>

class ModoA_HIL {
public:
    static void iniciar();
    static void ejecutar();
    static void detener();

private:
    static unsigned long tiempoPrevioHIL;
    static bool simulacionActiva;

    // Rutina de empaquetado y transmisión UART hacia Proteus
    static void enviarTramaProteus(int pwmA, int pwmB);
};

#endif // MODO_A_HIL_H
