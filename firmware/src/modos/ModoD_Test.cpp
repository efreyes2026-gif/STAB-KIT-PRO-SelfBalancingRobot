#include "ModoD_Test.h"
#include "../modulos/Inclinometro.h"
#include "../modulos/Encoders.h"
#include "../modulos/Motores.h"
#include "../modulos/HMI_OLED.h"

// Inicialización de variables de la rampa
ModoD_Test::EstadoRampa ModoD_Test::estadoRampa = ModoD_Test::RAMP_ADELANTE;
int ModoD_Test::pwmRampa = 0;
unsigned long ModoD_Test::timerRampa = 0;

void ModoD_Test::iniciar() {
    detener();
    pwmRampa = 0;
    estadoRampa = RAMP_ADELANTE;
    timerRampa = millis();
}

void ModoD_Test::detener() {
    Motores::frenar();
    pwmRampa = 0;
}

void ModoD_Test::ejecutar() {
    // Si el usuario acaba de oprimir un pulsador, frenamos motores por seguridad
    if (HMI_OLED::huboCambioDeEstado()) {
        detener();
        timerRampa = millis();
        estadoRampa = RAMP_ADELANTE;
    }

    // Ejecución según el submodo seleccionado con el botón Pin 33
    switch (HMI_OLED::obtenerSubModoD()) {
        case TEST_D_INCLINOMETRIA:
            detener();
            ejecutarInclinometria();
            break;

        case TEST_D_ENCODERS:
            detener();
            ejecutarEncoders();
            break;

        case TEST_D_MOTORES:
            ejecutarMotores();
            break;
    }
}

void ModoD_Test::ejecutarInclinometria() {
    // 1. Refrescar lecturas del MPU-6050 y GY-61
    Inclinometro::actualizar();

    // 2. Enviar datos a la OLED
    HMI_OLED::mostrarInclinometria(
        Inclinometro::obtenerPitchMPU(),
        Inclinometro::obtenerPitchGY61(),
        Inclinometro::obtenerRawGY61()
    );
}

void ModoD_Test::ejecutarEncoders() {
    // Enviar cuentas acumuladas por interrupción a la pantalla
    HMI_OLED::mostrarEncoders(
        Encoders::obtenerPulsosIzq(),
        Encoders::obtenerPulsosDer()
    );
}

void ModoD_Test::ejecutarMotores() {
    unsigned long ahora = millis();
    const int PASO_PWM = 5;
    const unsigned long INTERVALO_PASO = 35; // ms
    const unsigned long TIEMPO_FRENO = 1000; // ms

    switch (estadoRampa) {
        case RAMP_ADELANTE:
            if (ahora - timerRampa >= INTERVALO_PASO) {
                timerRampa = ahora;
                pwmRampa += PASO_PWM;
                if (pwmRampa >= 255) {
                    pwmRampa = 255;
                    estadoRampa = FRENO_ADELANTE;
                    timerRampa = ahora;
                }
                Motores::setVelocidades(pwmRampa, pwmRampa);
            }
            HMI_OLED::mostrarMotores("ADELANTE", pwmRampa, pwmRampa);
            break;

        case FRENO_ADELANTE:
            Motores::frenar();
            if (ahora - timerRampa >= TIEMPO_FRENO) {
                estadoRampa = RAMP_ATRAS;
                pwmRampa = 0;
                timerRampa = ahora;
            }
            HMI_OLED::mostrarMotores("FRENO", 0, 0);
            break;

        case RAMP_ATRAS:
            if (ahora - timerRampa >= INTERVALO_PASO) {
                timerRampa = ahora;
                pwmRampa += PASO_PWM;
                if (pwmRampa >= 255) {
                    pwmRampa = 255;
                    estadoRampa = FRENO_ATRAS;
                    timerRampa = ahora;
                }
                // Valores negativos comandan giro en reversa
                Motores::setVelocidades(-pwmRampa, -pwmRampa);
            }
            HMI_OLED::mostrarMotores("ATRAS", pwmRampa, pwmRampa);
            break;

        case FRENO_ATRAS:
            Motores::frenar();
            if (ahora - timerRampa >= TIEMPO_FRENO) {
                estadoRampa = RAMP_ADELANTE;
                pwmRampa = 0;
                timerRampa = ahora;
            }
            HMI_OLED::mostrarMotores("FRENO", 0, 0);
            break;
    }
}