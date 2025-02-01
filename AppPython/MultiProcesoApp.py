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
        self.lock = multiprocessing.Lock()

    def recibir_Transaccion(self, pedido, procesos):
        # Crear un proceso para cada pedido
        proceso = multiprocessing.Process(target=self.preparar_pedido, args=(pedido,))
        procesos.append(proceso)
        proceso.start()

    def preparar_pedido(self, pedido):
        with self.lock:
            print(f"dddddd: {pedido} - {multiprocessing.current_process().name}")
            time.sleep(random.randint(1, 5))  # Simula el tiempo de preparación
            print(f"Pedido listo: {pedido} - {multiprocessing.current_process().name}")


            



def main():
    PaymentProcess = ProcesadorPagos(3)  # 4 Canales concurrentes
    numTransactions = 10
    procesos = []

    start_time = time.time()  # Iniciar el timer


    for i in range(1, numTransactions + 1):
        pedido = f"Transacciones N:{i} "
        PaymentProcess.recibir_Transaccion(Transaccion(i, 100.50, "USD"), procesos)

    #PaymentProcess.recibir_Transaccion(Transaccion(2, 600.25, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(3, 350.25, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(4, 268.34, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(5, 167.26, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(6, 149.50, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(7, 268.13, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(8, 245.20, "USD"), procesos)
    #PaymentProcess.recibir_Transaccion(Transaccion(9, 469.15, "USD"), procesos)

    
    # Asegurarse de que todos los procesos terminen antes de calcular el tiempo total
  
    end_time = time.time()  # Finalizar el timer
    print(f"Tiempo total de Ejecución de todos los Pagos: {int((end_time - start_time) * 1000)} ms")

if __name__ == "__main__":
    main()