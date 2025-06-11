import os
import json

# Ruta de los archivos .txt
ruta_archivos = r"C:/Users/carlo/Desktop/Carlos/MasterLocal/VDSLocal"

def convertir_txt_a_json():
    for archivo_nombre in os.listdir(ruta_archivos):
        if archivo_nombre.endswith(".txt"):
            archivo_ruta = os.path.join(ruta_archivos, archivo_nombre)
            datos_json = []

            with open(archivo_ruta, 'r', encoding='utf-8') as archivo:
                # Nombres de los campos en el archivo JSON
                campos = [
                    "Square_id", "TimeInterval", "SMS_inactivity", "SMS_outactivity",
                    "Call_inactivity", "Call_outactivity", "Internet_traffic_activity", "Countrycode"
                ]

                # Leer cada línea del archivo
                for linea in archivo:
                    # Dividir la línea por tabuladores
                    valores = linea.strip().split("\t")

                    # Rellenar los valores faltantes con None (para que se convierta en null en JSON)
                    while len(valores) < len(campos):
                        valores.append(None)  # Usamos None para representar null en JSON

                    # Mapear los valores a sus campos correspondientes
                    datos_dict = {}
                    for i, valor in enumerate(valores):
                        # Asignar None para valores vacíos o faltantes, que se convertirán en null en JSON
                        if valor == "" or valor is None:
                            datos_dict[campos[i]] = None  # Esto se convertirá en null en JSON
                        else:
                            datos_dict[campos[i]] = valor

                    # Agregar el diccionario de esta línea a la lista de datos
                    datos_json.append(datos_dict)

            # Guardar los datos en un archivo JSON
            nuevo_nombre = archivo_nombre.replace(".txt", ".json")
            nuevo_ruta = os.path.join(ruta_archivos, nuevo_nombre)
            with open(nuevo_ruta, 'w', encoding='utf-8') as json_file:
                json.dump(datos_json, json_file, ensure_ascii=False, indent=4)
            print(f"{archivo_nombre} convertido a {nuevo_nombre}")

# Ejecutar la conversión
convertir_txt_a_json()
