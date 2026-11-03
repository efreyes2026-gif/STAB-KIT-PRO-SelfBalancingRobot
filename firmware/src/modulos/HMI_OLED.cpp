#include "HMI_OLED.h"
#include <Wire.h>
#include "ConfigHardware.h"
#include "ConfigLaboratorio.h"

Adafruit_SH1106G HMI_OLED::display = Adafruit_SH1106G(LAB_OLED_ANCHO, LAB_OLED_ALTO, &Wire, LAB_OLED_RESET);

ModoOperativo HMI_OLED::modoActual = MODO_D_TEST;
SubModoTestD  HMI_OLED::subModoD = TEST_D_INCLINOMETRIA;
TipoControl   HMI_OLED::tipoControlActual = CONTROL_PID;
bool          HMI_OLED::flagCambio = false;

unsigned long HMI_OLED::tiempoUltimoBtnPantalla = 0;
unsigned long HMI_OLED::tiempoUltimoBtnSistema = 0;
unsigned long HMI_OLED::tiempoUltimoRefresco = 0;

void HMI_OLED::iniciar() {
    pinMode(PIN_BTN_MODO, INPUT_PULLUP);     // Botón Pantalla (GPIO 32)
    pinMode(PIN_BTN_SUBMODO, INPUT_PULLUP);  // Botón Sistema  (GPIO 33)

    display.begin(LAB_OLED_ADDR, true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(15, 20);
    display.println("SISTEMA ROBOT");
    display.setCursor(15, 35);
    display.println("INICIANDO MODULOS...");
    display.display();
}

void HMI_OLED::actualizarBotones() {
    unsigned long ahora = millis();
    flagCambio = false;

    // 1. Botón PANTALLA (GPIO 32): Alterna Modo Principal (A -> B -> C -> D)
    if (digitalRead(PIN_BTN_MODO) == LOW && (ahora - tiempoUltimoBtnPantalla > LAB_DEBOUNCE_MS)) {
        tiempoUltimoBtnPantalla = ahora;
        modoActual = static_cast<ModoOperativo>((modoActual + 1) % 4);
        flagCambio = true;
        tiempoUltimoRefresco = 0;
        Serial.print(F("[HMI] Modo Principal cambiado a: "));
        Serial.println(modoActual);
    }

    // 2. Botón SISTEMA (GPIO 33): Conmuta leyes o submodos
    if (digitalRead(PIN_BTN_SUBMODO) == LOW && (ahora - tiempoUltimoBtnSistema > LAB_DEBOUNCE_MS)) {
        tiempoUltimoBtnSistema = ahora;
        flagCambio = true;
        tiempoUltimoRefresco = 0;

        if (modoActual == MODO_D_TEST) {
            // En Modo D: Submodos de prueba
            subModoD = static_cast<SubModoTestD>((subModoD + 1) % 3);
            Serial.print(F("[HMI] Modo D -> Submodo cambiado a: "));
            Serial.println(subModoD);
        } 
        // CORRECCIÓN: Permite cambiar control TANTO en Modo A (HIL) como en Modo B (Real)
        else if (modoActual == MODO_A_HIL || modoActual == MODO_B_REAL) {
            tipoControlActual = static_cast<TipoControl>((tipoControlActual + 1) % 3);
            Serial.print(F("[HMI] Ley de Control cambiada a: "));
            if (tipoControlActual == CONTROL_PID) Serial.println(F("PID"));
            else if (tipoControlActual == CONTROL_FUZZY) Serial.println(F("FUZZY"));
            else Serial.println(F("RNA (RED NEURONAL)"));
        }
    }
}

ModoOperativo HMI_OLED::obtenerModo() { return modoActual; }
SubModoTestD  HMI_OLED::obtenerSubModoD() { return subModoD; }
TipoControl   HMI_OLED::obtenerTipoControl() { return tipoControlActual; }
bool          HMI_OLED::huboCambioDeEstado() { return flagCambio; }

void HMI_OLED::mostrarStandby(const char* nombreModo) {
    if (millis() - tiempoUltimoRefresco < 150) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(10, 8);
    display.print(nombreModo);
    display.drawLine(0, 20, LAB_OLED_ANCHO, 20, SH110X_WHITE);

    display.setCursor(10, 32);
    display.print("Btn 32: Modo");
    display.setCursor(10, 46);
    display.print("Btn 33: Submodo");
    display.display();
}

void HMI_OLED::mostrarInclinometria(float pitchMPU, float pitchGY61, int rawGY61) {
    if (millis() - tiempoUltimoRefresco < 80) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 4);
    display.print("MODO D: INCLINOMETRIA");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(6, 18);
    display.print("PITCH MPU: ");
    display.print(pitchMPU, 1);
    display.print((char)247);

    display.setCursor(6, 32);
    display.print("PITCH GY : ");
    display.print(pitchGY61, 1);
    display.print((char)247);

    display.setCursor(6, 46);
    display.print("RAW ADC34: ");
    display.print(rawGY61);
    display.display();
}

void HMI_OLED::mostrarEncoders(long pulsosIzq, long pulsosDer) {
    if (millis() - tiempoUltimoRefresco < 100) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(16, 4);
    display.print("MODO D: ENCODERS");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(8, 20);
    display.print("ENC IZQ (26): ");
    display.print(pulsosIzq);

    display.setCursor(8, 34);
    display.print("ENC DER (27): ");
    display.print(pulsosDer);

    display.setCursor(8, 48);
    display.print("Gire ruedas a mano");
    display.display();
}

void HMI_OLED::mostrarMotores(const char* estado, int pwmA, int pwmB) {
    if (millis() - tiempoUltimoRefresco < 60) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(14, 4);
    display.print("MODO D: PUENTE H");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(8, 20);
    display.print("ESTADO: ");
    display.print(estado);

    display.setCursor(8, 34);
    display.print("PWM MOT A: ");
    display.print(pwmA);

    display.setCursor(8, 48);
    display.print("PWM MOT B: ");
    display.print(pwmB);
    display.display();
}

void HMI_OLED::mostrarModoA(const char* leyControl, float pitch, int pwm, bool activo) {
    if (millis() - tiempoUltimoRefresco < 60) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(6, 4);
    display.print("MODO A: HIL PROTEUS");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(6, 18);
    display.print("CONTROL: [");
    display.print(leyControl);
    display.print("]");

    display.setCursor(6, 30);
    display.print("PITCH  : ");
    display.print(pitch, 1);
    display.print((char)247);

    display.setCursor(6, 42);
    display.print("UART TX: M,");
    display.print(abs(pwm));

    display.setCursor(6, 54);
    if (activo) {
        display.print("ESTADO : HIL ACTIVO");
    } else {
        display.print("ESTADO : CAIDA (STOP)");
    }
    display.display();
}

void HMI_OLED::mostrarModoB(const char* leyControl, float pitch, int pwm, bool activo) {
    if (millis() - tiempoUltimoRefresco < 60) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 4);
    display.print("MODO B: EQUILIBRIO");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(6, 18);
    display.print("CONTROL: [");
    display.print(leyControl);
    display.print("]");

    display.setCursor(6, 30);
    display.print("PITCH  : ");
    display.print(pitch, 1);
    display.print((char)247);

    display.setCursor(6, 42);
    display.print("PWM OUT: ");
    display.print(pwm);

    display.setCursor(6, 54);
    if (activo) {
        display.print("ESTADO : ACTIVO (OK)");
    } else {
        display.print("ESTADO : CAIDA (STOP)");
    }
    display.display();
}

void HMI_OLED::mostrarModoC(float pitch, int pwm, bool wifiOk, unsigned long paquetesTx) {
    if (millis() - tiempoUltimoRefresco < 60) return;
    tiempoUltimoRefresco = millis();

    display.clearDisplay();
    display.drawRect(0, 0, LAB_OLED_ANCHO, LAB_OLED_ALTO, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(6, 4);
    display.print("MODO C: MATLAB UDP");
    display.drawLine(0, 14, LAB_OLED_ANCHO, 14, SH110X_WHITE);

    display.setCursor(6, 18);
    display.print("WIFI   : ");
    if (wifiOk) {
        display.print("[CONECTADO]");
    } else {
        display.print("[CONECTANDO...]");
    }

    display.setCursor(6, 30);
    display.print("PITCH  : ");
    display.print(pitch, 1);
    display.print((char)247);

    display.setCursor(6, 42);
    display.print("PWM    : ");
    display.print(pwm);

    display.setCursor(6, 54);
    display.print("PKTS TX: ");
    display.print(paquetesTx);

    display.display();
}