import firebase_admin
from firebase_admin import credentials, db
import pandas as pd
import time
from datetime import datetime
import os

# Inicializar la aplicación Firebase
cred = credentials.Certificate('C:/Users/romer/Escritorio/invernaderokey.json')
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://invernadero-7b818-default-rtdb.firebaseio.com/'  # Asegúrate de poner la URL de tu base de datos
})

# Función para leer datos de la ruta 'sensores/' en Realtime Database
def obtener_datos_realtime_database(ruta):
    ref = db.reference(ruta)
    data = ref.get()
    if data:
        df = pd.DataFrame([data])
        
        # Añadir columnas de fecha y hora actuales
        df['Fecha'] = datetime.now().strftime('%Y-%m-%d')  # Fecha en formato Año-Mes-Día
        df['Hora'] = datetime.now().strftime('%H:%M:%S')  # Hora en formato Horas:Minutos:Segundos
        
        return df
    else:
        return pd.DataFrame()  # Retorna un DataFrame vacío si no hay datos

# Función para agregar datos a un archivo CSV existente
def agregar_datos_csv(df, csv_file):
    if os.path.exists(csv_file):
        # Leer el archivo CSV existente
        df_existing = pd.read_csv(csv_file)
        # Añadir los nuevos datos al DataFrame existente
        df_combined = pd.concat([df_existing, df], ignore_index=True)
        df_combined.to_csv(csv_file, index=False)
    else:
        # Si el archivo no existe, crear uno nuevo con encabezados
        df.to_csv(csv_file, index=False)

# Ruta del archivo CSV y ruta de la base de datos de Firebase
csv_file = "C:/Users/romer/Escritorio/datos_firebase.csv"
ruta_database = "Sensores/"

# Bucle para leer y guardar los datos cada 5 minutos
while True:
    # Obtener datos de Firebase Realtime Database
    df = obtener_datos_realtime_database(ruta_database)

    # Agregar los datos al archivo CSV
    if not df.empty:
        agregar_datos_csv(df, csv_file)
        print(f"Datos agregados a {csv_file} exitosamente.")
    else:
        print("No se encontraron datos para agregar.")

    # Esperar 5 minutos (300 segundos)
    time.sleep(300)