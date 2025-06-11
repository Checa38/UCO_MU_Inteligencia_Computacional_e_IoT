import machine
import dht
import time
import network
import socket
import espnow

# Configurar la interfaz WLAN en modo estación
sta = network.WLAN(network.STA_IF)
sta.active(True)
#sta.disconnect()  # Solo necesario para ESP8266 si estaba conectado

# Inicializar ESPNow
e = espnow.ESPNow()
e.active(True)

# Agregar un peer (cambia la MAC por la de tu dispositivo receptor)
peer = b'\xe0\x5a\x1b\x5f\xd6\x5c'  # MAC address en formato binario
e.add_peer(peer)

# Enviar mensajes al peer
e.send(peer, "Starting...")

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
    e.send("x:{} y:{}".format(rec_x, rec_y))
    #Imprimir

    time.sleep(1)
    ## Actualizar cada 0.1 segundos.
    

e.send(peer, b'end')  # Envía el mensaje final
   
e.close()