#ifndef CONFIG_LABORATORIO_H
#define CONFIG_LABORATORIO_H

/* =========================================================================
 * CONSTANTES EXPERIMENTALES DE LABORATORIO Y PARÁMETROS BASE
 * ========================================================================= */

// --- 1. PANTALLA OLED SH1106 ---
#define LAB_OLED_ADDR          0x3C
#define LAB_OLED_ANCHO         128
#define LAB_OLED_ALTO          64
#define LAB_OLED_RESET         -1

// --- 2. SENSOR DIGITAL MPU-6050 ---
#define LAB_MPU_ADDR           0x68
#define LAB_MPU_ALPHA_FILTRO   0.98f  // Factor filtro complementario (0.98 giroscopio, 0.02 acelerómetro)

// --- 3. SENSOR ANALÓGICO GY-61 (Calibración Experimental Congelada) ---
#define LAB_GY61_ADC_MIN       1695   // Lectura a -90 grados
#define LAB_GY61_ADC_CENTRO    1795   // Lectura a 0 grados (equilibrio)
#define LAB_GY61_ADC_MAX       2009   // Lectura a +90 grados

// --- 4. CONFIGURACIÓN LEDC PWM (ESP32 para TB6612FNG) ---
#define LAB_PWM_FREQ           5000   // Frecuencia base: 5 kHz
#define LAB_PWM_RES_BITS       8      // Resolución: 8 bits (rango 0 a 255)
#define LAB_PWM_CANAL_A        0      // Canal LEDC para Motor A
#define LAB_PWM_CANAL_B        1      // Canal LEDC para Motor B

// --- 5. ODOMETRÍA BASE ---
#define LAB_ENCODER_PPR        48     // Pulsos nominales por revolución física

// --- 6. PARÁMETROS GENERALES DE TEMPORIZACIÓN ---
#define LAB_DEBOUNCE_MS        250    // Tiempo de estabilización de pulsadores
#define LAB_DT_CONTROL_MS      10     // Período de muestreo lazo de control (10 ms = 100 Hz)

// =========================================================================
// CONFIGURACIÓN DE RED Y TELEMETRÍA (MODO C - MATLAB UDP)
// =========================================================================
#define LAB_WIFI_SSID              "REYES 2024"         // Nombre de tu red Wi-Fi (2.4 GHz)
#define LAB_WIFI_PASS              "Sspacex$2024"       // Contraseña de tu red
//#define LAB_PC_IP                  "192.168.20.79"        // Dirección IP local de tu PC con MATLAB
#define LAB_PC_IP                  "192.168.20.255"       // Envío por difusión a toda la red local
#define LAB_UDP_PORT_DESTINO       5005                  // Puerto UDP donde escucha MATLAB
#define LAB_UDP_PORT_LOCAL         5006                  // Puerto local del ESP32

#endif // CONFIG_LABORATORIO_H

