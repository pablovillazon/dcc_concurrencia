#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <future>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <random>

class ThreadPool
{
    public:
        ThreadPool(size_t num_threads) : stop(false)
        {
            //Inicializar el threadpool con el numero de threads especificado
            for(size_t i = 0; i < num_threads; ++i)
            {
                threads.emplace_back([this] {
                    while(true)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(queue_mutex);
                            //Esperar a que haya una tarea en la cola
                            condition.wait(lock, [this] { return stop || !tasks.empty(); });
                            if(stop && tasks.empty())
                            {
                                return;
                            }
                            task = std::move(tasks.front());
                            tasks.pop();
                        }

                        //ejecutar la tarea
                        task();
                    }
                });
            }
        }

        //Agregar una tarea al threadpool
        template <class F>
        void enqueue(F&& f)
        {        
            {   
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.emplace(std::forward<F>(f));
            }
            condition.notify_one(); //Notificar a un thread que hay una tarea nueva
        }

        //Otra forma de agregar una tarea o un metodo Y de una clase X con un parametro s
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using return_type = typename std::result_of<F(Args...)>::type;
            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> res = task->get_future();            
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                
                if (stop) throw std::runtime_error("Enqueue on stopped ThreadPool");

                tasks.emplace([task]() { (*task)(); });
            }

            condition.notify_one();
            return res;
        }

        //Detener el threadpool
        void shutdown()
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true; //Indicar que se debe detener el threadpool
            }
            condition.notify_all(); //Notificar a todos los hilos que se debe detener

            for(std::thread& thread : threads)
            {
                thread.join();
            }
        }

    private:
        std::vector<std::thread> threads; //contenedor de threads
        std::queue<std::function<void()>> tasks; //cola de tareas
        std::mutex queue_mutex; //Mutex para proteger la cola de tareas
        std::condition_variable condition; //Condicion para notificar a los threads
        bool stop; // Indicador para detener el threadpool
};

// Clase Logger para manejar los logs
class Logger {
public:
    Logger(const std::string& filename) : logFile(filename, std::ios::app) {}
    ~Logger(){
        flush();
    }
    long logTimeElapsed(const std::string& activityName, std::chrono::steady_clock::time_point startTime) {
        auto endTime = std::chrono::steady_clock::now();
        auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::ostringstream oss;
        oss << "Tiempo transcurrido en " << activityName << ": " << timeElapsed << " ms";
        log(oss.str());
        return timeElapsed;
    }


    void log(const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
                << '.' << std::setw(3) << std::setfill('0') << now_ms.count()
                << " - " << message /*<< std::endl*/;
        messageQueue.push(oss.str());
    }

    void flush(){
        std::lock_guard<std::mutex> lock(queueMutex);
        while(!messageQueue.empty())
        {
            logFile << messageQueue.front() << std::endl;
            messageQueue.pop();
        }
    }

private:
    std::ofstream logFile;
    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
};

class Transaccion
{
    public:
        int id;
        double monto;
        std::string moneda;
};

class PasarelaDePagos
{
    public:
        PasarelaDePagos(Logger& logger) : logger(logger){}

        void procesarTransaccion(Transaccion transaccion)
        {                
            logger.log("Transaccion: " + std::to_string(transaccion.id) + "::" + std::to_string(transaccion.monto)+ "::"+ transaccion.moneda+ " RECIBIDA");
            auto startTime = std::chrono::steady_clock::now();
            //Procesar la Transaccion            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(2, 5);
            int secs = dis(gen);
            //Sleep por un tiempo aleatorio de n secs
            std::this_thread::sleep_for(std::chrono::seconds(secs));
            std::cout<<"Procesando la transaccion: "<<transaccion.id<<std::endl;
            
            logger.log("Transaccion: " + std::to_string(transaccion.id) + " PROCESADA");
            logger.logTimeElapsed("Tiempo de la Transaccion " + std::to_string(transaccion.id) , startTime);
            
        }
    private:
    Logger& logger;
};

int main(int argc, char* argv[])
{
    int nroTransacciones = 100;
    int nroThreads = 4;
    // Verificar si se ingresaron los argumentos
    if (argc == 3) {
        // Intentar convertir los argumentos a enteros
        try {
        nroThreads = std::stoi(argv[1]);
        nroTransacciones = std::stoi(argv[2]);
        } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Los argumentos no son números enteros." << std::endl;     
        return 1;
    }
    } else {
        // Establecer valores por defecto si no se ingresaron argumentos
        nroTransacciones = 100;
        nroThreads = 4;
        std::cout << "No se ingresaron argumentos. Se establecieron valores por defecto: " << nroThreads << " y " << nroTransacciones << std::endl;
    }


    Logger logger("log_v2.txt");
    auto startTime = std::chrono::steady_clock::now();
    logger.log("---Inicio de Ejecucion de Transacciones---");

    
    ThreadPool pool(nroThreads); //Inicializar un threadpool con 4 threads
    

    //Agregar tareas al threadpool
    /*
    pool.enqueue([] { std::cout<<"Tarea 1"<<std::endl; });   
    */

    //Procesando las transacciones
    PasarelaDePagos pasarela(logger);

    
    for (int i = 0; i < nroTransacciones; i++)
    {
        Transaccion transaccion{i+1, 10.99, "USD"};
        auto future = pool.enqueue(&PasarelaDePagos::procesarTransaccion, &pasarela, transaccion);
        //future.get();      
    }

    std::cout<<"la execucion esta en proceso!"<<std::endl;
    //Detener el threadpool
    

    pool.shutdown();
    std::cout<<"la execucion ha terminado!"<<std::endl;
    logger.log("---Fin de Ejecucion de Transacciones---");
    logger.log("Nro. de Threads: " + std::to_string(nroThreads));
    logger.log("Nro. Total de Transacciones atendidas: " + std::to_string(nroTransacciones));    
    auto tiempoTotal = logger.logTimeElapsed("Procesamiento de Transacciones", startTime);
    auto avgTransaccion = tiempoTotal / nroTransacciones;
    logger.log("Tiempo Promedio por Transaccion:" + std::to_string(avgTransaccion) + " ms");
    return 0;
}
