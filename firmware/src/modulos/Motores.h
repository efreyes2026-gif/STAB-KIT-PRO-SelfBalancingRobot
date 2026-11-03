#ifndef MODULO_MOTORES_H
#define MODULO_MOTORES_H

#include <Arduino.h>

class Motores {
public:
    // Configura los pines de dirección, canales LEDC PWM y LEDs indicadores
    static void iniciar();

    // Detiene ambos motores con freno activo (0-0) y apaga LEDs
    static void frenar();

    // Control individual por motor (rango admitido: -255 a 255)
    // Valores positivos: Giro adelante | Negativos: Reversa | Cero: Freno
    static void setVelocidadA(int pwm);
    static void setVelocidadB(int pwm);

    // Control simultáneo diferencial para ambos motores
    static void setVelocidades(int pwmA, int pwmB);

    // Obtener los valores actuales de PWM aplicados
    static int obtenerPwmA();
    static int obtenerPwmB();

private:
    static int pwmActualA;
    static int pwmActualB;
};

#endif // MODULO_MOTORES_H