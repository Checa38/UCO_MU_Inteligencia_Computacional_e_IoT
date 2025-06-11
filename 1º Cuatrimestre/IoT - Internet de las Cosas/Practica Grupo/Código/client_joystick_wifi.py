import machine
import dht
import time
import network
import socket
import espnow

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


Verti_y = ADC (Pin (33))

# Usar ADC para transmitir el "objeto" de un puerto.
# El objetivo es detectar la dirección de un joystick.
Verti_y.atten(ADC.ATTN_11DB)
# Cambiar la medición de voltaje a 3.6V.
# El objetivo es hacer que el joystick sea más "sensible". Así, incluso con un leve movimiento del joystick, se pueden obtener cambios en los datos. Si se utiliza el voltaje predeterminado de 1.0V, los cambios en los datos no serán tan evidentes con un toque suave. Mira la imagen a continuación.
ps2_x = ADC(Pin(32))
ps2_x.atten(ADC.ATTN_11DB)
while True:
    
    rec_y = Verti_y.read()
    # Recibir el valor recibido de "Y" en un bucle infinito.
    
    rec_x = ps2_x.read()
    # Recibir el valor recibido de "X" en un bucle infinito.
    
    print("x:{} y:{}".format(rec_x, rec_y))
    s.send("x:{} y:{}".format(rec_x, rec_y))
    #Imprimir

    time.sleep(1)
    ## Actualizar cada 0.1 segundos.
    

e.send(peer, b'end')  # Envía el mensaje final
   
e.close()