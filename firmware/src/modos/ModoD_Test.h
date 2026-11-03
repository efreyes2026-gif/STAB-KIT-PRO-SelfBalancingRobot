#ifndef MODO_D_TEST_H
#define MODO_D_TEST_H

#include <Arduino.h>

class ModoD_Test {
public:
    // Reinicia contadores y timers del modo de prueba
    static void iniciar();

    // Ejecuta el submodo activo seleccionado con el botón Pin 33
    static void ejecutar();

    // Detiene cualquier movimiento en curso
    static void detener();

private:
    // Sub-rutinas internas de prueba
    static void ejecutarInclinometria();
    static void ejecutarEncoders();
    static void ejecutarMotores();

    // Variables internas para la rampa no bloqueante de motores
    enum EstadoRampa { RAMP_ADELANTE, FRENO_ADELANTE, RAMP_ATRAS, FRENO_ATRAS };
    static EstadoRampa estadoRampa;
    static int pwmRampa;
    static unsigned long timerRampa;
};

#endif // MODO_D_TEST_H