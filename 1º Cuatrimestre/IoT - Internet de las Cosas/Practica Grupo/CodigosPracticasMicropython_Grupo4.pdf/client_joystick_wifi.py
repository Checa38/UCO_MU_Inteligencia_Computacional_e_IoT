import uasyncio as asyncio
from machine import ADC, Pin
import network
import espnow
import socket
import time
# Configurar la ESP32 como cliente
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.connect("IOTNET_2.4", "10T@ATC_")

# Esperar hasta que se conecte
while not sta.isconnected():
    print("Intentando conectar a la red...")
    time.sleep(1)

print("Conectado a la red con IP:", sta.ifconfig()[0])

e = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
e.connect(('192.168.2.26', 8080))  # Cambia 'IP_del_Servidor' por la IP de la ESP32 servidor

# Configurar los pines ADC para leer los valores del joystick
Verti_y = ADC(Pin(33))
Verti_y.atten(ADC.ATTN_11DB)

ps2_x = ADC(Pin(32))
ps2_x.atten(ADC.ATTN_11DB)

# Variables compartidas y evento para la sincronización
joystick_data = {"x": 0, "y": 0}
data_event = asyncio.Event()

# Función para verificar las esquinas
def verificar_esquina(x, y):
    if x > 4000 and y > 4000:
        return "Arriba-Izquierda"
    elif x < 100 and y < 100:
        return "Abajo-Derecha"
    elif x > 4000 and y < 100:
        return "Abajo-Izquierda"
    elif x < 100 and y > 4000:
        return "Arriba-Derecha"
    elif y < 100 :
        return "Abajo"
    elif y  > 4000 :
        return "Arriba"
    elif x < 100 :
        return "Derecha"
    elif x  > 4000 :
        return "Izquierda"
    else:
        return "Centro o en movimiento"
# Tarea para leer el joystick
async def leer_joystick():
    global joystick_data
    while True:
        rec_x = ps2_x.read()
        rec_y = Verti_y.read()
        joystick_data["x"] = rec_x
        joystick_data["y"] = rec_y
        print(f"Joystick -> x: {rec_x}, y: {rec_y}")
        print(verificar_esquina(rec_x, rec_y))
        
        # Disparar el evento indicando que hay nuevos datos
        data_event.set()
        await asyncio.sleep(1)  # Leer cada 100 ms

# Tarea para enviar datos por ESP-NOW
async def enviar_datos():
    while True:
        # Esperar hasta que haya datos nuevos
        await data_event.wait()
        data_event.clear()  # Limpiar el evento después de procesarlo
        
        # Enviar los datos por ESP-NOW
        
        mensaje = "x:{} y:{}".format(joystick_data["x"], joystick_data["y"])
        #mensaje = verificar_esquina(rec_x, rec_y)
        e.send(mensaje)
        #print(f"Enviado -> {mensaje}")
        
        await asyncio.sleep(1)  # Intervalo opcional

# Tarea principal
async def main():
    e.send( "Starting...")  # Mensaje inicial
    print("Comenzando tareas...")
    
    # Crear tareas asincrónicas
    tarea_joystick = asyncio.create_task(leer_joystick())
    tarea_envio = asyncio.create_task(enviar_datos())
    
    # Mantener la ejecución
    await asyncio.gather(tarea_joystick, tarea_envio)

# Ejecutar el bucle principal de asyncio
try:
    asyncio.run(main())
except KeyboardInterrupt:
    e.send( b'end')  # Envía el mensaje final
    e.close()
    print("Programa terminado.")
