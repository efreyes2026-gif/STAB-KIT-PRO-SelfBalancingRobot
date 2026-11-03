import socket
import serial
import time
import sys

# =========================================================================
# CONFIGURACIÓN DEL ENLACE HIL
# =========================================================================
TCP_HOST = '127.0.0.1'
TCP_PORT = 4000
COM_PORT = 'COM7'        # Conectado al extremo 1 del par VSPE (COM1 <-> COM2)
BAUDRATE = 115200

print("=" * 60)
print(f"[*] INICIANDO PUENTE HIL: Wokwi (TCP {TCP_PORT}) <---> Proteus ({COM_PORT})")
print("=" * 60)

# 1. Apertura del Puerto Serial COM1
try:
    ser_proteus = serial.Serial(COM_PORT, BAUDRATE, timeout=0.01)
    print(f"[+] Puerto serie {COM_PORT} abierto a {BAUDRATE} baudios con exito.")
except Exception as e:
    print(f"[!] ERROR al abrir {COM_PORT}: {e}")
    print("    -> Verifica que VSPE este corriendo con el par 'COM1 <-> COM2'.")
    sys.exit(1)

# 2. Conexion al Socket TCP de Wokwi (con reintentos automaticos)
sock_wokwi = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print(f"[*] Conectando a Wokwi en {TCP_HOST}:{TCP_PORT}...")
conectado = False
while not conectado:
    try:
        sock_wokwi.connect((TCP_HOST, TCP_PORT))
        sock_wokwi.setblocking(False)  # Modo no bloqueante para maxima velocidad
        conectado = True
        print("[+] Enlace TCP con Wokwi establecido correctamente.")
    except socket.error:
        print("[.] Esperando que inicie la simulacion en Wokwi (reintentando en 1s)...")
        time.sleep(1.0)

print("\n[+] PUENTE HIL ACTIVO: Transmitiendo datos bidireccionales. Presiona Ctrl+C para salir.\n")

# 3. Lazo de Reenvio Bidireccional de Ultra Baja Latencia
try:
    while True:
        # A. Direccion Wokwi (ESP32) -> Proteus (COM1 -> COM2 -> ATmega328P)
        try:
            data_w = sock_wokwi.recv(1024)
            if data_w:
                ser_proteus.write(data_w)
                ser_proteus.flush()
        except BlockingIOError:
            pass  # No hay datos entrantes de Wokwi en este instante
        except socket.error as e:
            print(f"[!] Error en socket TCP Wokwi: {e}")
            break

        # B. Direccion Proteus (ATmega328P) -> Wokwi (ESP32)
        try:
            if ser_proteus.in_waiting > 0:
                data_p = ser_proteus.read(ser_proteus.in_waiting)
                if data_p:
                    sock_wokwi.sendall(data_p)
        except Exception as e:
            print(f"[!] Error leyendo {COM_PORT}: {e}")
            break

        # Pausa minima para no saturar el hilo de CPU
        time.sleep(0.002)

except KeyboardInterrupt:
    print("\n[*] Deteniendo puente HIL por el usuario...")
finally:
    try:
        ser_proteus.close()
        sock_wokwi.close()
    except Exception:
        pass
    print("[*] Conexiones cerradas.")