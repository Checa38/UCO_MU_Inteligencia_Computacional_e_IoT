import network
import time
import socket

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
    if addr[0] == ip_permitida:
        
        # Recibir datos
        data = conn.recv(1024)
        print("Mensaje recibido:", data.decode())
        
        #conn.close()  # Cerrar la conexión después de recibir el mensaje
        #break  # Salir después de recibir el primer mensaje
    else:
        print("Conexión rechazada desde:", addr[0])
        conn.close()  # Cerrar conexión si no coincide con la IP permitida



