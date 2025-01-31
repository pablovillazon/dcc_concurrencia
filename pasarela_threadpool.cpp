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
    
    void log(const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        logFile << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
                << '.' << std::setw(3) << std::setfill('0') << now_ms.count()
                << " - " << message << std::endl;
    }

private:
    std::ofstream logFile;
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
            
            //Procesar la Transaccion
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout<<"Procesando la transaccion: "<<transaccion.id<<std::endl;
            
            logger.log("Transaccion: " + std::to_string(transaccion.id) + " PROCESADA");
            
        }
    private:
    Logger& logger;
};

int main()
{
    Logger logger("log.txt");
    ThreadPool pool(4); //Inicializar un threadpool con 4 threads

    //Agregar tareas al threadpool
    
    pool.enqueue([] { std::cout<<"Tarea 1"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 2"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 3"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 4"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 5"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 6"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 7"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 8"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 9"<<std::endl; });
    
    //Procesando las transacciones
    PasarelaDePagos pasarela(logger);
    Transaccion transaccion{1, 10.99, "USD"};
    Transaccion transaccion2{2, 10.99, "USD"};
    Transaccion transaccion3{3, 10.99, "USD"};
    //std::cout<<transaccion.moneda<<std::endl;
    auto future = pool.enqueue(&PasarelaDePagos::procesarTransaccion, &pasarela, transaccion);
    auto future2 = pool.enqueue(&PasarelaDePagos::procesarTransaccion, &pasarela, transaccion2);
    auto future3 = pool.enqueue(&PasarelaDePagos::procesarTransaccion, &pasarela, transaccion3);
    
    future.get();
    future2.get();
    future3.get();    
    

    std::cout<<"la execution, ha terminado!"<<std::endl;
    //Detener el threadpool
    pool.shutdown();
}
