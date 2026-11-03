#ifndef CONTROL_PID_H
#define CONTROL_PID_H

#include <Arduino.h>

class ControlPID {
public:
    // Inicializa variables internas y resetea integrador
    static void iniciar();

    // Reinicia el acumulador integral y el error previo
    static void resetear();

    // Calcula el esfuerzo de control en función del ángulo y dt
    // Retorna: Valor PWM entre -255 y +255 (Signo = Dirección de giro)
    static int calcular(float anguloActual, float dt);

    // Métodos para reconfigurar ganancias dinámicamente si se requiere
    static void configurarGanancias(float kp, float ki, float kd);

private:
    static float kp;
    static float ki;
    static float kd;
    static float errorPrevio;
    static float terminoIntegral;
};

#endif // CONTROL_PID_H