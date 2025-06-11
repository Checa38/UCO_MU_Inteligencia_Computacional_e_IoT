import network
import espnow
import socket
from machine import Pin, PWM

# Conexión a la red Wi-Fi
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.connect("IOTNET_2.4", "10T@ATC_")

# Esperar hasta que se conecte a Wi-Fi
while not sta.isconnected():
    print("Intentando conectar a la red...")
    time.sleep(1)

print("Conectado a la red con IP:", sta.ifconfig()[0])

# IP específica desde la que permitiremos conexiones
ip_permitida = "192.168.2.29"  # Cambia esta IP por la IP de la ESP32 cliente permitida

# Crear y configurar el servidor TCP/IP

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((sta.ifconfig()[0], 8080))  # Escuchar en la IP asignada de la ESP32 servidor y el puerto 8080
s.listen(1)

print("Esperando conexión en la IP:", sta.ifconfig()[0])
conn, addr = s.accept()
while True:

    # Verificar si la IP de la conexión entrante es la permitida
   
        
        # Recibir datos
        data = conn.recv(1024)
        print("Mensaje recibido:", data.decode())
        
        #conn.close()  # Cerrar la conexión después de recibir el mensaje
        #break  # Salir después de recibir el primer mensaje

# Configurar los pines PWM para el LED RGB
led_r = PWM(Pin(25))  # Rojo
led_g = PWM(Pin(26))  # Verde
led_b = PWM(Pin(27))  # Azul

# Ajustar la frecuencia del PWM
led_r.freq(500)
led_g.freq(500)
led_b.freq(500)

# Función para mapear un valor de un rango a otro
def map_value(value, in_min, in_max, out_min, out_max):
    return int((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

# Función para actualizar el color del LED RGB según x e y
def actualizar_rgb(x, y):
    # Mapear los valores de x e y a un rango de 0 a 1023 (intensidad de PWM)
    r = map_value(x, 0, 4095, 0, 1023)  # Rojo depende de x
    b = map_value(y, 0, 4095, 0, 1023)  # Azul depende de y
    g = map_value((x + y) // 2, 0, 4095, 0, 1023)  # Verde depende de la mezcla de x e y

    # Ajustar los colores del LED
    led_r.duty(r)
    led_g.duty(g)
    led_b.duty(b)

    print(f"RGB -> R: {r}, G: {g}, B: {b}")


print("Esperando mensajes...")

# Bucle principal para recibir datos y controlar el LED RGB
while True:
    if addr[0] == ip_permitida:
        msg = conn.recv(1024)
        print("Mensaje recibido:", msg.decode())

            # Decodificar el mensaje y extraer los valores de x e y
        try:
            data = msg.decode('utf-8')  # Decodificar el mensaje
            if data.startswith("x:") and "y:" in data:
                    # Parsear los valores de x e y
                valores = data.replace("x:", "").replace("y:", "").split()
                x = int(valores[0])
                y = int(valores[1])

                    # Actualizar el color del LED RGB
                actualizar_rgb(x, y)
        except Exception as ex:
            print(f"Error al procesar el mensaje: {ex}")
    else:
        print("Conexión rechazada desde:", addr[0])
        conn.close()  # Cerrar conexión si no coincide con la IP permitida
        

