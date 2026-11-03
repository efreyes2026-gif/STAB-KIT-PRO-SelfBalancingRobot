#ifndef MODULO_HMI_OLED_H
#define MODULO_HMI_OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Modos Operativos Principales (Conmutados con Botón PANTALLA - Pin 32)
enum ModoOperativo {
    MODO_A_HIL = 0,
    MODO_B_REAL,
    MODO_C_MATLAB,
    MODO_D_TEST
};

// Submodos del Banco de Pruebas (Conmutados con Botón SISTEMA - Pin 33 en Modo D)
enum SubModoTestD {
    TEST_D_INCLINOMETRIA = 0,
    TEST_D_ENCODERS,
    TEST_D_MOTORES
};

// Leyes de Control de Equilibrio en Modo B (Conmutadas con Botón SISTEMA - Pin 33)
enum TipoControl {
    CONTROL_PID = 0,
    CONTROL_FUZZY,
    CONTROL_RNA
};

class HMI_OLED {
public:
    static void iniciar();
    static void actualizarBotones();

    static ModoOperativo obtenerModo();
    static SubModoTestD  obtenerSubModoD();
    static TipoControl   obtenerTipoControl();
    static bool          huboCambioDeEstado();

    // Rutinas de renderizado gráfico
    static void mostrarStandby(const char* nombreModo);
    static void mostrarInclinometria(float pitchMPU, float pitchGY61, int rawGY61);
    static void mostrarEncoders(long pulsosIzq, long pulsosDer);
    static void mostrarMotores(const char* estado, int pwmA, int pwmB);
    static void mostrarModoA(const char* leyControl, float pitch, int pwm, bool activo);
    static void mostrarModoB(const char* leyControl, float pitch, int pwm, bool activo);
    static void mostrarModoC(float pitch, int pwm, bool wifiOk, unsigned long paquetesTx); // <-- NUEVO

private:
    static Adafruit_SH1106G display;
    static ModoOperativo    modoActual;
    static SubModoTestD     subModoD;
    static TipoControl      tipoControlActual;
    static bool             flagCambio;

    static unsigned long    tiempoUltimoBtnPantalla;
    static unsigned long    tiempoUltimoBtnSistema;
    static unsigned long    tiempoUltimoRefresco;
};

#endif // MODULO_HMI_OLED_H