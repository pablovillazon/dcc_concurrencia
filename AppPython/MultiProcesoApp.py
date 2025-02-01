import multiprocessing
import time
import random
import json
from datetime import datetime
from colorama import Fore, Style, init

# Inicializar colorama para colores en la terminal
init(autoreset=True)

class Transaccion:
    """Representa una transacción bancaria."""
    def __init__(self, id: int, monto: float, moneda: str):
        self.id = id
        self.monto = monto
        self.moneda = moneda
        self.estado = "Pendiente"  # Estado inicial

    def __str__(self):
        return f"Transacción(ID: {self.id}, Monto: {self.monto} {self.moneda}, Estado: {self.estado})"

class ProcesadorPagos:
    """Sistema de procesamiento de pagos con concurrencia."""
    def __init__(self, num_concurrencia):
        self.num_concurrencia = num_concurrencia
        self.manager = multiprocessing.Manager()
        self.lock = self.manager.Lock()  # Protección contra race conditions
        self.transacciones_completadas = self.manager.list()  # Lista compartida entre procesos

        # Creamos un pool de procesos con nombres personalizados
        self.pool = multiprocessing.Pool(processes=num_concurrencia, initializer=self.init_worker, initargs=(self.lock,))

    @staticmethod
    def init_worker(lock):
        """Inicializa cada proceso con un lock y un nombre personalizado."""
        global proceso_lock
        proceso_lock = lock
        current_process = multiprocessing.current_process()
        current_process.name = f"Proceso-Pagos-{current_process._identity[0]}"  # Nombre personalizado

    def recibir_transaccion(self, transaccion):
        """Recibe una transacción y la envía al procesamiento asincrónico."""
        self.pool.apply_async(self.procesar_transaccion, args=(transaccion, self.lock, self.transacciones_completadas))

    @staticmethod
    def procesar_transaccion(transaccion, lock, transacciones_completadas):
        """Ejecuta el procesamiento de la transacción de manera segura con lock."""
        process_name = multiprocessing.current_process().name  # Obtener nombre del proceso
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")  # Timestamp actual

        with lock:
            print(Fore.CYAN + f"🕒 [{timestamp}] {process_name} - INICIO Procesando: {transaccion}")

        time.sleep(random.uniform(1, 4))  # Simula tiempo de procesamiento

        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")  # Timestamp final
        transaccion.estado = "Completada"
        with lock:
            print(Fore.GREEN + f"✅ [{timestamp}] {process_name} - FINALIZADO: {transaccion}")
            transacciones_completadas.append(transaccion)

    def generar_reporte(self):
        """Muestra un resumen de las transacciones procesadas y el total de pagos."""
        total_procesado = sum(t.monto for t in self.transacciones_completadas)

        print("\n" + Fore.YELLOW + "=" * 60)
        print(Fore.YELLOW + "📊 **RESUMEN DE TRANSACCIONES PROCESADAS** 📊")
        print(Fore.YELLOW + "=" * 60)
        for transaccion in self.transacciones_completadas:
            print(Fore.LIGHTWHITE_EX + f"- {transaccion}")

        print(Fore.YELLOW + "=" * 60)
        print(Fore.LIGHTGREEN_EX + f"💰 TOTAL PROCESADO: {total_procesado} USD")
        print(Fore.YELLOW + "=" * 60)

def cargar_transacciones_desde_json(archivo_json):
    """Carga transacciones desde un archivo JSON."""
    with open(archivo_json, "r") as file:
        data = json.load(file)
    return [Transaccion(t["id"], t["monto"], t["moneda"]) for t in data]

def main():
    """Ejecuta el procesamiento concurrente de transacciones bancarias."""
    procesador_pagos = ProcesadorPagos(4)  # 4 Procesos de pago

    start_time = time.time()  # Iniciar el cronómetro

    # Cargar transacciones desde JSON
    transacciones = cargar_transacciones_desde_json("./AppPython/transacciones_multimoneda.json")

    for transaccion in transacciones:
        procesador_pagos.recibir_transaccion(transaccion)

    procesador_pagos.pool.close()
    procesador_pagos.pool.join()

    end_time = time.time()  # Finalizar el cronómetro
    print(Fore.MAGENTA + f"\n⏳ TIEMPO TOTAL DE EJECUCIÓN: {int((end_time - start_time) * 1000)} ms\n")

    procesador_pagos.generar_reporte()

if __name__ == "__main__":
    main()
