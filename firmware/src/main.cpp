#include <Arduino.h>
#include <Wire.h>

// 1. Configuraciones Globales
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"
#include "ConfigTuning.h"

// 2. Módulos de Hardware Encapsulados
#include "modulos/Inclinometro.h"
#include "modulos/Motores.h"
#include "modulos/Encoders.h"
#include "modulos/HMI_OLED.h"

// 3. Capa de Control y Modos Operativos
#include "control/ControlPID.h"
#include "control/ControlFuzzy.h"
#include "control/ControlRNA.h"
#include "modos/ModoB_Real.h"
#include "modos/ModoD_Test.h"
#include "modos/ModoA_HIL.h"
#include "modos/ModoC_Matlab.h"

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // Invocación del Setup por Módulo
    HMI_OLED::iniciar();
    Inclinometro::iniciar();
    Motores::iniciar();
    Encoders::iniciar();

    // Inicialización de Controladores
    ControlPID::iniciar();
    ControlFuzzy::iniciar();
    ControlRNA::iniciar();

    // Inicialización de Modos
    ModoB_Real::iniciar();
    ModoD_Test::iniciar();
    ModoA_HIL::iniciar();
    ModoC_Matlab::iniciar();

    Serial.println(F("[SISTEMA] Modo B Completo: PID, FUZZY y RNA Operativos."));
}

void loop() {
    HMI_OLED::actualizarBotones();

    switch (HMI_OLED::obtenerModo()) {
        case MODO_A_HIL:
            ModoA_HIL::ejecutar();
            break;

        case MODO_B_REAL:
            ModoB_Real::ejecutar();
            break;

        case MODO_C_MATLAB:
            ModoC_Matlab::ejecutar();
            break;

        case MODO_D_TEST:
            ModoD_Test::ejecutar();
            break;
    }

    delay(1);
}