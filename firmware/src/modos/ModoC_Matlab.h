#ifndef MODO_C_MATLAB_H
#define MODO_C_MATLAB_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class ModoC_Matlab {
public:
    static void iniciar();
    static void ejecutar();
    static void detener();
    static bool estaConectadoWifi();

private:
    static WiFiUDP socketUDP;
    static unsigned long tiempoPrevioUDP;
    static unsigned long tiempoInicioCaptura;
    static unsigned long tiempoUltimoIntentoWifi;
    static unsigned long contadorPaquetes;
    static float errorPrevio;
    static bool wifiConectado;

    static void intentarConectarWifi();
    static void transmitirTelemetria(float pitch, float dPitch, long encIzq, long encDer, int pwm);
};

#endif // MODO_C_MATLAB_H