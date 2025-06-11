"""
Tutorial micropython
Parte 1
-------
Ejecución del primer script en micropython

LEDS

fernando.leon@uco.es
University of Córdoba, Spain
2021
"""


# importación de librerías
from machine import Pin
from time import sleep

# variable global
led = Pin(2, Pin.OUT)

# código secuencial
#while True:
led.on()
sleep(1)
led.off()
sleep(1)




"""
Tutorial micropython
Parte 2
-------
Conexión con un dispositivo externo

BUZZER pinout:

- -> GND
+ -> GPIO4


fernando.leon@uco.es
University of Córdoba, Spain
2021
"""

# importación de librerías
from machine import Pin
from time import sleep
from buzzer import Buzzer

# variables globales
led = Pin(2, Pin.OUT)
buzzer = Buzzer(4)

# código secuencial
while True:
    led.on()
    buzzer.beep(200,1)
    led.off()
    sleep(1)


"""
Tutorial micropython
Parte 3
-------
Pulsación de botón mediante pooling (sondeo)

BUZZER pinout:

- -> GND
S -> GPIO4

BUTTON pinout:
- -> GND
S -> GPIO13


fernando.leon@uco.es
University of Córdoba, Spain
2021
"""

# importación de librerías
from machine import Pin
from time import sleep_ms
from buzzer import Buzzer

# variables globales
led = Pin(2, Pin.OUT)
buzzer = Buzzer(4)
button = Pin(13, Pin.IN, Pin.PULL_UP)


# código secuencial
while True:
    
    if button.value() == False:
        led.on()
        buzzer.beep(200,1)
    else:
        led.off()
        
    sleep_ms(100)
    

"""
Tutorial micropython
Parte 4
-------
Pulsación de botón mediante interrupción.
NOTA: Probar a pulsar el botón varias veces mientras se escucha el beep -> Encolamiento. 

BUZZER pinout:

- -> GND
S -> GPIO4

BUTTON pinout:
- -> GND
S -> GPIO13


fernando.leon@uco.es
University of Córdoba, Spain
2021
"""

# importación de librerías
from machine import Pin
from time import sleep_ms
from buzzer import Buzzer

# variables globales
led = Pin(2, Pin.OUT)
buzzer = Buzzer(4)
button = Pin(13, Pin.IN, Pin.PULL_UP)

# manejador de interrupción (IRQ = Interruption ReQuest)
def irq_button(pin):
    led.on()
    buzzer.beep(200,1)
    led.off()

# código secuencial
button.irq(trigger=Pin.IRQ_FALLING, handler=irq_button)

# NOP
while True:        
    sleep_ms(100)
    
"""
Tutorial micropython
Parte 5
-------
Interrupción para activación/desactivación de beep.

NOTA: observar el error a causa del contexto de la variable "on"

BUZZER pinout:

- -> GND
S -> GPIO4

BUTTON pinout:
- -> GND
S -> GPIO13


fernando.leon@uco.es
University of Córdoba, Spain
2021
"""

# importación de librerías
from machine import Pin
from time import sleep_ms
from buzzer import Buzzer

# variables globales
led = Pin(2, Pin.OUT)
buzzer = Buzzer(4)
button = Pin(13, Pin.IN, Pin.PULL_UP)
on = False

# manejador de interrupción (IRQ = Interruption ReQuest)
def irq_button(pin):
    #global on
    if not on:
        led.on()
        buzzer.beep_on(200)
        on = True
    else:
        buzzer.beep_off()
        led.off()
        on = False

# código secuencial
button.irq(trigger=Pin.IRQ_FALLING, handler=irq_button)

# NOP
while True:        
    sleep_ms(100)

