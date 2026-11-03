#ifndef CONFIG_HARDWARE_H
#define CONFIG_HARDWARE_H

/* =========================================================================
 * ASIGNACIÓN OFICIAL DE PINES - ESP32 DEVKIT V1
 * Proyecto: Robot Autoequilibrado de Dos Ruedas
 * ========================================================================= */

// --- 1. BUS I2C COMPARTIDO (MPU-6050 y OLED SH1106) ---
#define PIN_I2C_SDA         21
#define PIN_I2C_SCL         22

// --- 2. SENSOR ANALÓGICO GY-61 (ADC1) ---
#define PIN_GY61_X          34    // Eje X (ADC1 Canal 6)
#define PIN_GY61_Y          35    // Eje Y (ADC1 Canal 7)

// --- 3. PUENTE H DUAL TB6612FNG ---
// Motor A (Izquierdo)
#define PIN_PWMA            5     // Control de velocidad PWM
#define PIN_AIN1            19    // Dirección 1
#define PIN_AIN2            18    // Dirección 2

// Motor B (Derecho)
#define PIN_PWMB            23    // Control de velocidad PWM
#define PIN_BIN1            16    // Dirección 1
#define PIN_BIN2            17    // Dirección 2

// Nota: Pin STBY conectado físicamente a 3.3V (Habilitado continuo)

// --- 4. ODOMETRÍA / ENCODERS (Interrupciones por Hardware) ---
#define PIN_ENC_IZQ         26    // Encoder Rueda Izquierda
#define PIN_ENC_DER         27    // Encoder Rueda Derecha

// --- 5. INTERFAZ HMI (Pulsadores con INPUT_PULLUP a GND) ---
#define PIN_BTN_MODO        32    // Botón PANTALLA (Azul): Alterna Modo A, B, C, D
#define PIN_BTN_SUBMODO     33    // Botón SISTEMA (Amarillo): Navega dentro del modo
#define PIN_BTN_CONTROL     13    // Botón OPERADOR (Verde): Reservado selector PID/Fuzzy/RNA

// --- 6. INDICADORES VISUALES (LEDs hacia GND) ---
#define PIN_LED_PWM_A       25    // LED Verde: Intensidad PWM Motor A
#define PIN_LED_PWM_B       14    // LED Verde: Intensidad PWM Motor B
#define PIN_LED_DIR_A       2     // LED Azul: Sentido Motor A
#define PIN_LED_DIR_B       4     // LED Azul: Sentido Motor B

#endif // CONFIG_HARDWARE_H