#ifndef MODO_B_REAL_H
#define MODO_B_REAL_H

#include <Arduino.h>

class ModoB_Real {
public:
    static void iniciar();
    static void ejecutar();
    static void detener();

private:
    static unsigned long tiempoPrevioControl;
    static bool robotEnPie;
};

#endif // MODO_B_REAL_H