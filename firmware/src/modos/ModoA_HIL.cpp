#include "ModoA_HIL.h"
#include "../modulos/Inclinometro.h"
#include "../modulos/Motores.h"
#include "../modulos/HMI_OLED.h"
#include "../control/ControlPID.h"
#include "../control/ControlFuzzy.h"
#include "../control/ControlRNA.h"
#include "ConfigTuning.h"
#include "ConfigLaboratorio.h"

unsigned long ModoA_HIL::tiempoPrevioHIL = 0;
bool ModoA_HIL::simulacionActiva = false;

void ModoA_HIL::iniciar() {
    detener();
    tiempoPrevioHIL = millis();
}

void ModoA_HIL::detener() {
    // 1. Asegurar que los pines físicos del TB6612FNG estén totalmente en reposo
    Motores::frenar();

    // 2. Enviar trama de freno total a Proteus
    enviarTramaProteus(0, 0);

    ControlPID::resetear();
    ControlFuzzy::resetear();
    ControlRNA::resetear();
    simulacionActiva = false;
}

void ModoA_HIL::enviarTramaProteus(int pwmA, int pwmB) {
    // Formato robusto: M,pwmA,dirA,pwmB,dirB\n
    // dir: 1 = Adelante, 0 = Atrás
    int dirA = (pwmA >= 0) ? 1 : 0;
    int dirB = (pwmB >= 0) ? 1 : 0;
    int absA = constrain(abs(pwmA), 0, 255);
    int absB = constrain(abs(pwmB), 0, 255);

    char bufferTrama[32];
    snprintf(bufferTrama, sizeof(bufferTrama), "M,%d,%d,%d,%d\n", absA, dirA, absB, dirB);
    Serial.print(bufferTrama);
}

void ModoA_HIL::ejecutar() {
    // Al conmutar modos o cambiar ley de control
    if (HMI_OLED::huboCambioDeEstado()) {
        ControlPID::resetear();
        ControlFuzzy::resetear();
        ControlRNA::resetear();
    }

    unsigned long ahora = millis();

    // Lazo periódico estricto a 100 Hz (dt = 10 ms)
    if (ahora - tiempoPrevioHIL >= LAB_DT_CONTROL_MS) {
        float dt = (ahora - tiempoPrevioHIL) / 1000.0f;
        tiempoPrevioHIL = ahora;

        // 1. Lectura del ángulo
        Inclinometro::actualizar();
        float pitchActual = Inclinometro::obtenerPitchMPU();

        // 2. Selección de ley de control (conmutable con Botón Sistema 33)
        TipoControl tipo = HMI_OLED::obtenerTipoControl();
        const char* nombreLey = "PID";
        if (tipo == CONTROL_FUZZY) nombreLey = "FUZZY";
        else if (tipo == CONTROL_RNA) nombreLey = "RNA";

        // 3. Verificación de Kill-Switch ante caída
        if (abs(pitchActual) > TUNING_ANGULO_CAIDA_MAX) {
            detener();
            HMI_OLED::mostrarModoA(nombreLey, pitchActual, 0, false);
            return;
        }

        simulacionActiva = true;

        // 4. Cálculo del esfuerzo según algoritmo activo
        int esfuerzoPWM = 0;
        switch (tipo) {
            case CONTROL_PID:
                esfuerzoPWM = ControlPID::calcular(pitchActual, dt);
                break;
            case CONTROL_FUZZY:
                esfuerzoPWM = ControlFuzzy::calcular(pitchActual, dt);
                break;
            case CONTROL_RNA:
                esfuerzoPWM = ControlRNA::calcular(pitchActual, dt);
                break;
        }

        // 5. Envío HIL: Transmite por UART hacia bridge.py y Proteus
        enviarTramaProteus(esfuerzoPWM, esfuerzoPWM);

        // 6. Retroalimentación en OLED dedicada a Modo A
        HMI_OLED::mostrarModoA(nombreLey, pitchActual, esfuerzoPWM, true);
    }
}