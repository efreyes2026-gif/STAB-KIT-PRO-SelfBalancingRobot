#ifndef CONTROL_RNA_H
#define CONTROL_RNA_H

#include <Arduino.h>

class ControlRNA {
public:
    // Inicializa variables internas
    static void iniciar();

    // Reinicia el estado del error previo
    static void resetear();

    // Calcula el esfuerzo de control por inferencia directa (Feedforward)
    // Entradas: ángulo actual (grados) y dt (segundos)
    // Retorna: PWM entre -255 y +255
    static int calcular(float anguloActual, float dt);

private:
    static float errorPrevio;

    // Función de activación no lineal
    static float funcionTanh(float x);
};

#endif // CONTROL_RNA_H