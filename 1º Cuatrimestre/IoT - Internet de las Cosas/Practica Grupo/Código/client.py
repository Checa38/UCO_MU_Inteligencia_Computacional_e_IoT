import machine
import dht
import time
import network
import socket

# Configurar la ESP32 como cliente
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.connect("IOTNET_2.4", "10T@ATC_")

# Esperar hasta que se conecte
while not sta.isconnected():
    print("Intentando conectar a la red...")
    time.sleep(1)

print("Conectado a la red con IP:", sta.ifconfig()[0])

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.2.25', 8080))  # Cambia 'IP_del_Servidor' por la IP de la ESP32 servidor

# Enviar mensaje
#s.send(b"Hola desde la ESP32 cliente!")

# Configurar el pin del sensor (cambiar el número de pin según tu conexión)
pin_sensor = machine.Pin(2)  # Aquí usamos el pin GPIO 4
sensor = dht.DHT11(pin_sensor)  # Crear una instancia del sensor DHT11

while True:
    try:
        # Leer el sensor
        sensor.measure()
       
        # Obtener la temperatura y la humedad
        temperatura = sensor.temperature()  # Obtener temperatura en °C
        humedad = sensor.humidity()  # Obtener humedad en %

        # Imprimir los resultados
        s.send("Temperatura: " + str(temperatura) + " °C" + "\n" + "Humedad: " + str(humedad) + "%")
        #s.send("Humedad:", humedad, "%")
       
    except OSError as e:
        print("Error al leer el sensor:", e)
   
    # Esperar 2 segundos antes de la próxima lectura

    time.sleep(1)
   
s.close()