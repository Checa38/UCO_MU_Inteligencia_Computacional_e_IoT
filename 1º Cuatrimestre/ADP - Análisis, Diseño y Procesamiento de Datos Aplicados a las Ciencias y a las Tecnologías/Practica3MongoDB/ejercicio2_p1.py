from pymongo import MongoClient
from bson.son import SON
# Conectar a MongoDB
try:
    cliente = MongoClient("mongodb://localhost:27017/")
    db = cliente["Milan_CDR_db"]
    coleccion = db["Milan_CDR_c"]
    print("Conexión a MongoDB exitosa.")
except Exception as e:
    print(f"Error al conectar a MongoDB: {e}")
    exit(1)

pipeline = [
    {"$group": {"_id": "$Country_code"}},
]

distinct_countries = coleccion.aggregate(pipeline)
distinct_countries = [doc['_id'] for doc in distinct_countries]
print("Países con los que se interactúa:", distinct_countries)