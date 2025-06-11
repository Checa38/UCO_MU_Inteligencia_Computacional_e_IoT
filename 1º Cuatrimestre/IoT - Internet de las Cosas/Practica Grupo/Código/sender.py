import network
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
for i in range(100):
    e.send(peer, "test")  # Envía datos largos
e.send(peer, b'end')  # Envía el mensaje final
