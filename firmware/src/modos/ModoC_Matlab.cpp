#include "ModoC_Matlab.h"
#include "../modulos/Inclinometro.h"
#include "../modulos/Encoders.h"
#include "../modulos/Motores.h"
#include "../modulos/HMI_OLED.h"
#include "../control/ControlPID.h"
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"
#include "ConfigTuning.h"

WiFiUDP ModoC_Matlab::socketUDP;
unsigned long ModoC_Matlab::tiempoPrevioUDP = 0;
unsigned long ModoC_Matlab::tiempoInicioCaptura = 0;
unsigned long ModoC_Matlab::tiempoUltimoIntentoWifi = 0;
unsigned long ModoC_Matlab::contadorPaquetes = 0;
float ModoC_Matlab::errorPrevio = 0.0f;
bool ModoC_Matlab::wifiConectado = false;

// Dirección IP del computador destino parseada nativamente
static IPAddress ipDestinoPC;

void ModoC_Matlab::iniciar() {
    detener();
    tiempoUltimoIntentoWifi = 0;
    contadorPaquetes = 0;
    ipDestinoPC.fromString(LAB_PC_IP);
}

void ModoC_Matlab::detener() {
    Motores::frenar();
    ControlPID::resetear();
}

void ModoC_Matlab::intentarConectarWifi() {
    // Si ya está físicamente conectado, asegurar el socket y salir
    if (WiFi.status() == WL_CONNECTED) {
        if (!wifiConectado) {
            wifiConectado = true;
            WiFi.setSleep(false); // <-- OBLIGATORIO: Mantiene la radio a máxima potencia y evita que se congele el búfer
            socketUDP.begin(LAB_UDP_PORT_LOCAL);
            Serial.println(F("\n[WIFI] Enlace establecido con exito."));
    }
    return;
}


    // Si perdió la conexión, marcar desconectado para congelar los envíos UDP
    wifiConectado = false;
    unsigned long ahora = millis();

    // Solo reintentar asociar si pasaron al menos 6 segundos y NO está conectando
    if (ahora - tiempoUltimoIntentoWifi >= 6000) {
        tiempoUltimoIntentoWifi = ahora;
        Serial.print(F("[WIFI] Intentando conectar a: "));
        Serial.println(LAB_WIFI_SSID);
        
        WiFi.disconnect(true);
        delay(10);
        WiFi.mode(WIFI_STA);
        WiFi.begin(LAB_WIFI_SSID, LAB_WIFI_PASS);
    }
}

bool ModoC_Matlab::estaConectadoWifi() {
    return (WiFi.status() == WL_CONNECTED);
}

void ModoC_Matlab::transmitirTelemetria(float pitch, float dPitch, long encIzq, long encDer, int pwm) {
    if (WiFi.status() != WL_CONNECTED || !wifiConectado) return;

    unsigned long tiempoRelativo = millis() - tiempoInicioCaptura;
    char bufferUDP[64];
    int longitud = snprintf(bufferUDP, sizeof(bufferUDP), "%lu,%.2f,%.2f,%ld,%ld,%d\n",
                            tiempoRelativo, pitch, dPitch, encIzq, encDer, pwm);

    IPAddress ipPC;
    if (ipPC.fromString(LAB_PC_IP)) {
        socketUDP.beginPacket(ipPC, LAB_UDP_PORT_DESTINO);
        socketUDP.write((const uint8_t*)bufferUDP, longitud);
        if (socketUDP.endPacket() == 1) {
            contadorPaquetes++;
        }
    }
    yield(); // Permite al procesador del ESP32 vaciar la cola de transmisión de la antena
}

void ModoC_Matlab::ejecutar() {
    intentarConectarWifi();

    unsigned long ahora = millis();

    if (ahora - tiempoPrevioUDP >= LAB_DT_CONTROL_MS) {
        float dt = (ahora - tiempoPrevioUDP) / 1000.0f;
        tiempoPrevioUDP = ahora;

        Inclinometro::actualizar();
        float pitchActual = Inclinometro::obtenerPitchMPU();
        long encIzq = Encoders::obtenerPulsosIzq();
        long encDer = Encoders::obtenerPulsosDer();

        float setpoint = TUNING_SETPOINT_ANGULO + TUNING_OFFSET_CENTRO_MASA;
        float error = setpoint - pitchActual;
        float dError = (error - errorPrevio) / dt;
        errorPrevio = error;

        int esfuerzoPWM = 0;
        if (abs(pitchActual) <= TUNING_ANGULO_CAIDA_MAX) {
            esfuerzoPWM = ControlPID::calcular(pitchActual, dt);
            Motores::setVelocidades(esfuerzoPWM, esfuerzoPWM);
        } else {
            detener();
        }

        transmitirTelemetria(pitchActual, dError, encIzq, encDer, esfuerzoPWM);
        HMI_OLED::mostrarModoC(pitchActual, esfuerzoPWM, wifiConectado, contadorPaquetes);
    }
}