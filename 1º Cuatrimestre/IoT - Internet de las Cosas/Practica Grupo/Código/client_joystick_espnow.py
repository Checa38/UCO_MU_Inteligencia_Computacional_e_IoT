import uasyncio as asyncio
from machine import ADC, Pin
import network
import espnow

# Configurar la interfaz WLAN en modo estación
sta = network.WLAN(network.STA_IF)
sta.active(True)

# Inicializar ESPNow
e = espnow.ESPNow()
e.active(True)

# Agregar un peer (cambia la MAC por la de tu dispositivo receptor)
peer = b'\xe0\x5a\x1b\x5f\xd6\x5c'  # MAC address en formato binario
e.add_peer(peer)

# Configurar los pines ADC para leer los valores del joystick
Verti_y = ADC(Pin(33))
Verti_y.atten(ADC.ATTN_11DB)

ps2_x = ADC(Pin(32))
ps2_x.atten(ADC.ATTN_11DB)

# Variables compartidas y eventos
joystick_data = {"x": 0, "y": 0}
data_event = asyncio.Event()
sequence_event = asyncio.Event()

# Secuencia esperada y registro de movimientos
expected_sequence = ["Arriba", "Abajo", "Izquierda", "Derecha"]
current_sequence = []

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
        direction = verificar_esquina(rec_x, rec_y)
        print(f"Joystick -> x: {rec_x}, y: {rec_y}")
        print(direction)
        
        # Registrar movimientos para la secuencia
        if direction in expected_sequence:
            current_sequence.append(direction)
            if len(current_sequence) > len(expected_sequence):
                current_sequence.pop(0)  # Mantener el tamaño limitado
            
            # Comprobar si coincide con la secuencia esperada
            if current_sequence == expected_sequence:
                sequence_event.set()
        
        # Disparar el evento indicando que hay nuevos datos
        data_event.set()
        await asyncio.sleep(2)  # Leer cada 100 ms

# Tarea para enviar datos por ESP-NOW
async def enviar_datos():
    while True:
        # Esperar hasta que haya datos nuevos
        await data_event.wait()
        data_event.clear()  # Limpiar el evento después de procesarlo
        
        # Enviar los datos por ESP-NOW
        mensaje = verificar_esquina(joystick_data["x"], joystick_data["y"])
        e.send(peer, mensaje)
        await asyncio.sleep(2)  # Intervalo opcional

# Tarea para comprobar la secuencia
async def detectar_secuencia():
    while True:
        # Esperar a que se detecte la secuencia
        await sequence_event.wait()
        sequence_event.clear()  # Limpiar el evento después de procesarlo
        
        # Acciones al detectar la secuencia
        print("¡Secuencia detectada: Arriba, Abajo, Izquierda, Derecha!")
        e.send(peer, "¡Secuencia detectada!")

# Tarea principal
async def main():
    e.send(peer, "Starting...")  # Mensaje inicial
    print("Comenzando tareas...")
    
    # Crear tareas asincrónicas
    tarea_joystick = asyncio.create_task(leer_joystick())
    tarea_envio = asyncio.create_task(enviar_datos())
    tarea_secuencia = asyncio.create_task(detectar_secuencia())
    
    # Mantener la ejecución
    await asyncio.gather(tarea_joystick, tarea_envio, tarea_secuencia)

# Ejecutar el bucle principal de asyncio
try:
    asyncio.run(main())
except KeyboardInterrupt:
    e.send(peer, b'end')  # Envía el mensaje final
    e.close()
    print("Programa terminado.")
