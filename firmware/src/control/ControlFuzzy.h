#ifndef CONTROL_FUZZY_H
#define CONTROL_FUZZY_H

#include <Arduino.h>

class ControlFuzzy {
public:
    // Inicializa variables internas
    static void iniciar();

    // Reinicia el estado previo de error
    static void resetear();

    // Calcula el esfuerzo de control difuso
    // Entradas: ángulo actual y dt
    // Retorna: PWM entre -255 y +255
    static int calcular(float anguloActual, float dt);

private:
    static float errorPrevio;

    // Funciones de pertenencia triangulares/trapezoidales
    static float pertenenciaTrapecio(float x, float a, float b, float c, float d);
    static float pertenenciaTriangulo(float x, float a, float b, float c);
};

#endif // CONTROL_FUZZY_H