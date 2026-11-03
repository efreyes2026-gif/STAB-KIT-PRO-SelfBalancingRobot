#ifndef CONFIG_TUNING_H
#define CONFIG_TUNING_H

/* =========================================================================
 * PARÁMETROS DE SINTONIZACIÓN Y AJUSTE POR EL USUARIO (TUNING)
 * ========================================================================= */

// --- 1. REFERENCIA Y LÍMITES DE SEGURIDAD (KILL-SWITCH) ---
#define TUNING_SETPOINT_ANGULO     0.0f    // Ángulo de equilibrio vertical deseado (grados)
#define TUNING_OFFSET_CENTRO_MASA  0.0f    // Calibración fina si el peso de la batería desvía el centro
#define TUNING_ANGULO_CAIDA_MAX    35.0f   // Si supera |35°|, frena motores por seguridad (caída)

/*// --- 2. COMPENSACIÓN DE FRICCIÓN (DEADBAND) ---
#define TUNING_DEADBAND_PWM        30      // PWM mínimo para vencer la inercia estática de las reductoras*/

// --- 3. GANANCIAS DEL CONTROLADOR PID ---
#define TUNING_PID_KP              18.0f   // Ganancia Proporcional (reacción rápida al ángulo)
#define TUNING_PID_KI              0.0f    // Ganancia Integral (elimina error estacionario, iniciar en 0)
#define TUNING_PID_KD              1.2f    // Ganancia Derivativa (amortiguamiento por velocidad angular)

// Límites del término integral para evitar saturación acumulativa (Anti-Windup)
#define TUNING_PID_INTEGRAL_MAX    100.0f

/*// --- 4. FACTORES DE ESCALA CONTROL DIFUSO (FUZZY) ---
#define TUNING_FUZZY_GE            83.72f    // Factor de escala para el Error de ángulo
#define TUNING_FUZZY_GDE           99.11f    // Factor de escala para la Derivada del error
#define TUNING_FUZZY_GU            4.785f    // Factor de escala para el Esfuerzo de salida*/


// =========================================================================
// PARÁMETROS DE SINTONIZACIÓN DEL CONTROLADOR DIFUSO (FLC)
// =========================================================================

// Factor de escala del Error (Rigidez angular)
// Si el robot tarda en reaccionar o se vence ante perturbaciones, subir este valor (ej. 1.2f a 1.5f).
#define TUNING_FUZZY_GE             1.0f

// Factor de escala de la Derivada (Amortiguamiento / Freno)
// Si el robot oscila rápidamente (vibra) alrededor de 0°, subir GDE (ej. 1.0f a 1.4f).
// Si reacciona rígido y brusco, bajar GDE (ej. 0.6f a 0.8f).
#define TUNING_FUZZY_GDE            0.85f

// Factor de escala de Salida (Fuerza global de empuje PWM)
// Ganancia maestra del esfuerzo de control hacia el puente H.
#define TUNING_FUZZY_GU             1.0f

// Compensación de Zona Muerta (Deadband de motores y reductor)
// PWM mínimo necesario para que las ruedas comiencen a girar venciendo la fricción.
// Típicamente entre 25 y 40 para motores N20 o amarillos TT.
#define TUNING_DEADBAND_PWM         30


#endif // CONFIG_TUNING_H