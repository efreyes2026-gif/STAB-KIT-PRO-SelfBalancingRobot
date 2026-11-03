#include "ModoB_Real.h"
#include "../modulos/Inclinometro.h"
#include "../modulos/Encoders.h"
#include "../modulos/Motores.h"
#include "../modulos/HMI_OLED.h"
#include "../control/ControlPID.h"
#include "../control/ControlFuzzy.h"
#include "../control/ControlRNA.h"
#include "ConfigTuning.h"
#include "ConfigLaboratorio.h"

unsigned long ModoB_Real::tiempoPrevioControl = 0;
bool ModoB_Real::robotEnPie = false;

void ModoB_Real::iniciar() {
    detener();
    ControlPID::resetear();
    ControlFuzzy::resetear();
    ControlRNA::resetear();
    tiempoPrevioControl = millis();
    robotEnPie = false;
}

void ModoB_Real::detener() {
    Motores::frenar();
    ControlPID::resetear();
    ControlFuzzy::resetear();
    ControlRNA::resetear();
    robotEnPie = false;
}

void ModoB_Real::ejecutar() {
    if (HMI_OLED::huboCambioDeEstado()) {
        ControlPID::resetear();
        ControlFuzzy::resetear();
        ControlRNA::resetear();
    }

    unsigned long ahora = millis();

    // Lazo de control determinístico a 100 Hz (dt = 10 ms)
    if (ahora - tiempoPrevioControl >= LAB_DT_CONTROL_MS) {
        float dt = (ahora - tiempoPrevioControl) / 1000.0f;
        tiempoPrevioControl = ahora;

        // 1. Adquisición del ángulo por fusión de sensores
        Inclinometro::actualizar();
        float pitchActual = Inclinometro::obtenerPitchMPU();

        // 2. Determinar la ley de control activa
        TipoControl tipo = HMI_OLED::obtenerTipoControl();
        const char* nombreLey = "PID";
        if (tipo == CONTROL_FUZZY) nombreLey = "FUZZY";
        else if (tipo == CONTROL_RNA) nombreLey = "RNA";

        // 3. Kill-Switch de seguridad ante caída
        if (abs(pitchActual) > TUNING_ANGULO_CAIDA_MAX) {
            detener();
            HMI_OLED::mostrarModoB(nombreLey, pitchActual, 0, false);
            return;
        }

        robotEnPie = true;

        // 4. Cálculo del esfuerzo según la ley seleccionada
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

        // 5. Aplicar salida directa al puente H TB6612FNG
        Motores::setVelocidades(esfuerzoPWM, esfuerzoPWM);

        // 6. Refresco en la pantalla OLED
        HMI_OLED::mostrarModoB(nombreLey, pitchActual, esfuerzoPWM, true);
    }
}