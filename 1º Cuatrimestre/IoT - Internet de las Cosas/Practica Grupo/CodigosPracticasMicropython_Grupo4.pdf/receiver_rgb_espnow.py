import network
import espnow
from machine import Pin, PWM

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

# Configurar la interfaz WLAN en modo estación
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()  # Desconectar de cualquier punto de acceso previo

# Inicializar ESPNow
e = espnow.ESPNow()
e.active(True)

print("Esperando mensajes...")

# Bucle principal para recibir datos y controlar el LED RGB
while True:
    host, msg = e.recv()  # Recibir datos
    if msg:  # Procesar el mensaje solo si no es `None`
        print(f"Mensaje recibido de {host}: {msg}")
        if msg == b'end':  # Detener el programa si se recibe "end"
            print("Finalizando programa...")
            break
        
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

