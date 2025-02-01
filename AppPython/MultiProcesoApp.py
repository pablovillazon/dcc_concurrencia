import multiprocessing
import random
import time

class Transaccion:
    def __init__(self, id: int, monto: float, moneda: str):
        self.id = id
        self.monto = monto
        self.moneda = moneda

    def __str__(self):
        return f"Transacción(ID: {self.id}, Monto: {self.monto}, Moneda: {self.moneda})"



class ProcesadorPagos:
    def __init__(self, num_concurrencia):
        self.num_concurrencia = num_concurrencia
        self.pool = multiprocessing.Pool(processes=num_concurrencia)
        manager = multiprocessing.Manager()
        self.lock = manager.Lock()  # Shared lock across processes

    def recibir_Transaccion(self, transaccion):
        # Crear un proceso para cada transaccion        
        self.pool.apply_async(self.preparar_pedido, args=(transaccion, self.lock))

    @staticmethod
    def preparar_pedido(pedido, lock):
        with lock:
            print(f"Preparando: {pedido} - {multiprocessing.current_process().name}")
        time.sleep(2)  # Simula el tiempo de preparación
        with lock:
            print(f"Pedido listo: {pedido} - {multiprocessing.current_process().name}")



    def preparar_transaccion(self, transaccion):
        with self.lock:
            print(f"Añadiendo al Pool: {transaccion.id} - {multiprocessing.current_process().name}")
            time.sleep(3)  # Simula el tiempo de preparación
            print(f"Pedido listo: {pedido} - {multiprocessing.current_process().name}")


            



def main():
    PaymentProcess = ProcesadorPagos(4)  # 4 Canales concurrentes
    numTransactions = 10
    procesos = []

    start_time = time.time()  # Iniciar el timer


    #for i in range(1, numTransactions + 1):
    #    pedido = f"Transacciones N:{i} "
    PaymentProcess.recibir_Transaccion(Transaccion(1, 100.50, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(2, 600.25, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(3, 350.25, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(4, 268.34, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(5, 167.26, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(6, 149.50, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(7, 268.13, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(8, 245.20, "USD"))
    PaymentProcess.recibir_Transaccion(Transaccion(9, 469.15, "USD"))

     # Asegurarse de que todos los procesos terminen antes de calcular el tiempo total
    #for proceso in procesos:
    #    proceso.join()

      # Properly close and join the pool before exiting
    PaymentProcess.pool.close()
    #PaymentProcess.pool.join()

    
    # Asegurarse de que todos los procesos terminen antes de calcular el tiempo total
  
    end_time = time.time()  # Finalizar el timer
    print(f"Tiempo total de Ejecución de todos los Pagos: {int((end_time - start_time) * 1000)} ms")

if __name__ == "__main__":
    main()