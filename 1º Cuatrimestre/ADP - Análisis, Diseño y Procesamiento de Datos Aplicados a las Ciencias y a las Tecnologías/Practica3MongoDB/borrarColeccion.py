# Importar la librería de MongoDB
from pymongo import MongoClient

# Conectar al cliente de MongoDB
client = MongoClient('mongodb://localhost:27017/')  # Cambia la URL si es necesario

# Seleccionar la base de datos
database = client['Milan_CDR_db']

# Eliminar la colección
database.drop_collection('Milan_CDR_c')

print("Colección Milan_CDR_c eliminada con éxito.")
