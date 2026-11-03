#ifndef MODULO_INCLINOMETRO_H
#define MODULO_INCLINOMETRO_H

#include <Arduino.h>

class Inclinometro {
public:
    // Inicializa el sensor MPU-6050 y la calibración base
    static void iniciar();

    // Actualiza lecturas y filtro complementario (llamar en cada ciclo)
    static void actualizar();

    // Métodos públicos para obtener ángulos calculados
    static float obtenerPitchMPU();
    static float obtenerPitchGY61();
    static int   obtenerRawGY61();

private:
    static float anguloPitchMPU;
    static float anguloPitchGY61;
    static int   rawGY61;
    static unsigned long tiempoPrevio;
};

#endif // MODULO_INCLINOMETRO_H