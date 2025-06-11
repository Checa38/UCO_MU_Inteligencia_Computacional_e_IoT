import os
import numpy as np
import json
import pymongo
import warnings
warnings.filterwarnings('ignore')

FILES_PATH = r"C:/Users/carlo/Desktop/Carlos/MasterLocal/ADPLocal/JSONs/"
client = pymongo.MongoClient('mongodb://localhost:27017')
database = client['Milan_CDR_db']

collist = database.list_collection_names()
if "Milan_CDR_c" in collist:
    print("The collection Milan_CDR_c exists.")
    Milan_CDR_c = database.get_collection("Milan_CDR_c")
else:
    # Primero creo la colección Milan_CDR_c
    database.create_collection("Milan_CDR_c")
    
    Milan_CDR_c = database.get_collection("Milan_CDR_c")

    for file in os.listdir(FILES_PATH):
        # Inserto cada archivo en la colección
        try:
            with open(FILES_PATH + file, encoding="utf-8") as f:
                file_data = json.load(f)
                Milan_CDR_c.insert_many(file_data)
        except UnicodeDecodeError as e:
            print(f"Error de codificación en el archivo: {file} - {e}")
        except json.JSONDecodeError as e:
            print(f"Error en el formato JSON del archivo: {file} - {e}")
