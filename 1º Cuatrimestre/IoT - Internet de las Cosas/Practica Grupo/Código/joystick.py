import network
import time
import socket
from machine import ADC, Pin

Verti_y = ADC (Pin (33))

# Usar ADC para transmitir el "objeto" de un puerto.
# El objetivo es detectar la dirección de un joystick.
Verti_y.atten(ADC.ATTN_11DB)

# Cambiar la medición de voltaje a 3.6V.
# El objetivo es hacer que el joystick sea más "sensible". Así, incluso con un leve movimiento del joystick, se pueden obtener cambios en los datos. Si se utiliza el voltaje predeterminado de 1.0V, los cambios en los datos no serán tan evidentes con un toque suave. Mira la imagen a continuación.
ps2_x = ADC(Pin(32))

ps2_x.atten(ADC.ATTN_11DB)

# Cuando presionas el joystick hacia abajo, "press" recibirá los datos.
#Press = Pin(15, Pin. IN)
while True:
    
    rec_y = Verti_y.read()
    # Recibir el valor recibido de "Y" en un bucle infinito.
    
    rec_x = ps2_x.read()
    # Recibir el valor recibido de "X" en un bucle infinito.
    
    print("x:{} y:{}".format(rec_x, rec_y))
    #Imprimir

    time.sleep(1)
    ## Actualizar cada 0.1 segundos.
